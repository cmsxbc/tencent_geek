from dataclasses import dataclass, field
from typing import List, Callable, Any, NewType, TypeVar


T = TypeVar('T')
UNDEFINED = None


class _LazyProp:
    def __init__(self, parent=None, name=None, **props):
        assert not (parent is None) ^ (name is None)
        self.parent = parent
        self.name = name
        self._props = props
        self._prototype = self.__class__

    def __getitem__(self, item):
        return _LazyProp(self, item)

    def __setitem__(self, key, value):
        self._props[key] = value
        if isinstance(value, _LazyProp):
            value.parent = self
            value.name = key

    def __str__(self):
        return f'({self.str_relation()}){{{self.str_props()}}}'

    def str_props(self):
        return ', '.join(f"{k!s}:{v!s}" for k, v in self._props.items())

    def str_relation(self):
        if self.parent is None:
            return 'global'
        return f'{self.parent.str_relation()}::{self.name!s}'

    def eval(self):
        if self.parent is None:
            item = {}
        else:
            if isinstance(self.parent, _LazyProp):
                parent = self.parent.eval()
            else:
                parent = self.parent
            item = parent[self.name]
        for n, v in self._props.items():
            if isinstance(v, _LazyProp):
                v = v.eval()
            item[n] = v
        return item


class _Array(_LazyProp):
    def __init__(self, parent, name, **props):
        super().__init__(parent, name)
        for k, v in props.items():
            self[int(k)] = v

    def __setitem__(self, key, value):
        key = int(key)
        super().__setitem__(key, value)

    def __str__(self):
        return f'{{{self.str_props()}}}'

    def __len__(self):
        return len(self._props)

    def __iter__(self):
        return self._props.values()


class _Function(_LazyProp):
    def __init__(self, func, parent=None, name=None):
        self._func = func
        super().__init__(parent, name)

    def __call__(self, *args, **kwargs):
        return self._func(*args, **kwargs)


class _Window(_LazyProp):

    OBJ_MAPPING = {
        'Array': lambda *values: _Array(parent=None, name=None, **{str(i): v for i, v in enumerate(values)})
    }

    def __init__(self, parent=None, name=None):
        super().__init__(parent, name, window=self, **self.OBJ_MAPPING)

    def __del__(self):
        self._props.clear()  # for self reference

    def __class_getitem__(cls, item):
        if item not in cls.OBJ_MAPPING:
            raise NotImplementedError("not implement")
        return cls.OBJ_MAPPING[item]


@dataclass
class VM:
    stack: List = field(default_factory=list)
    window: _Window = field(default_factory=_Window)

    def pop(self):
        return self.stack.pop()

    def slice(self, n):
        assert n > 0, "you sb!!, slice"
        ret = self.stack[-n:]
        self.stack = self.stack[:-n]
        return ret

    def dup(self):
        return self.stack.append(self.stack[-1])

    def push(self, val):
        return self.stack.append(val)

    def peek(self):
        return self.get(-1)

    def set(self, val, p=-1):
        self.stack[p] = val

    def eval_set(self, func: Callable[[T], T], p=-1):
        self.stack[p] = func(self.stack[p])

    def get(self, p):
        return self.stack[p]

    def swap(self, n):
        assert n >= 0, "you sb!!, swap"
        t = -2 - n
        self.stack[-1], self.stack[t] = self.stack[t], self.stack[-1]

    def depth(self):
        return len(self.stack)

    def clear(self):
        self.stack = []

    def scale(self, n, absolute=True):
        delta = n
        if absolute:
            delta -= len(self.stack)
        if delta < 0:
            self.stack = self.stack[:delta]
        else:
            self.stack += [None] * delta

    def pretty_print(self):
        print('=========')
        print('stack top')
        print('=========')
        for v in self.stack[::-1]:
            print(v)
        print('------------')
        print('stack bottom')
        print('------------')

    def __iter__(self):
        for i in self.stack:
            yield i

    def array(self, *args, **kwargs):
        return _Window['Array'](*args, **kwargs)


VmFunc = NewType('VmFunc', Callable[[VM, List[int]], Any])


def default_vm_func(vm, ops):
    raise NotImplementedError


def pop(vm: VM, ops):
    vm.pop()


def push(vm: VM, ops):
    vm.push(ops[0])


def set_top(vm: VM, ops):
    vm.set(ops[0])


def push_empty_str(vm: VM, ops):
    vm.push("")


def push_undefined(vm: VM, ops):
    vm.push(UNDEFINED)


def push_false(vm: VM, ops):
    vm.push(False)


def push_true(vm: VM, ops):
    vm.push(True)


def dup(vm: VM, ops):
    vm.dup()


def swap(vm: VM, ops):
    vm.swap(ops[0])


def append_chr(vm: VM, ops):
    def append_str(s):
        return s + chr(ops[0] + vm.peek())
    vm.eval_set(append_str, -2)


def add(vm: VM, ops):
    a = vm.pop()
    vm.eval_set(lambda x: a + x)


def sub(vm: VM, ops):
    a = vm.pop()
    vm.eval_set(lambda x: x - a)


def mul(vm: VM, ops):
    a = vm.pop()
    vm.eval_set(lambda x: x * a)


def shr(vm: VM, ops):
    a = vm.pop()
    vm.eval_set(lambda x: x >> a)


def shl(vm: VM, ops):
    a = vm.pop()
    vm.eval_set(lambda x: x << a)


def combine(vm: VM, ops):
    # vm.push([vm.pop(), vm.pop()][::-1])
    v = vm.pop()
    vm.eval_set(lambda x: vm.array(x, v))


def combine_prop(vm: VM, ops):
    v = vm.pop()
    vm.eval_set(lambda x: vm.array(x[0][x[1]], v))


def prop_set_nopop(vm: VM, ops):
    def set_prop(x):
        x[0][x[1]] = vm.peek()
        return x
    vm.eval_set(set_prop, -2)


def prop_set_pop_val(vm: VM, ops):
    val = vm.pop()

    def set_prop(x):
        x[0][x[1]] = val

    vm.eval_set(set_prop)


def window_prop_getter(vm: VM, ops):
    vm.eval_set(lambda x: vm.array(vm.window, x))


def get_prop(vm: VM, ops):
    vm.eval_set(lambda x: x[0][x[1]])


def call(vm: VM, ops):
    args = vm.slice(ops[0]) if ops[0] > 0 else ()
    vm.eval_set(lambda x: x(*args))


def mk_array(vm: VM, ops):
    vm.eval_set(lambda x: vm.array(x))
