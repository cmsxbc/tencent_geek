from dataclasses import dataclass, field
from typing import List, Callable, Any, NewType, TypeVar


T = TypeVar('T')
UNDEFINED = None


class LazyProp:
    def __init__(self, obj, prop=None):
        self.obj = obj
        self.prop = prop

    def __getitem__(self, item):
        return LazyProp(self, item)


class _Window(LazyProp):

    OBJ_MAPPING = {
        'Array': lambda *values: {i: v for i, v in enumerate(values)}
    }

    def __init__(self):
        self._data = {
            'window': self
        }
        self._data.update(self.OBJ_MAPPING)

    def __del__(self):
        self._data.clear()  # for self reference

    def __getitem__(self, item):
        if item not in self._data:
            raise IndexError(f"unknown item: {item}")
        return self._data[item]

    def __setitem__(self, key, value):
        if key in self.OBJ_MAPPING:
            raise NotImplementedError("modify default mapping is not supported!")
        self._data[key] = value

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
    vm.push([vm.pop(), vm.pop()][::-1])


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
    vm.eval_set(lambda x: _Window['Array'](vm.window, x))


def get_prop(vm: VM, ops):
    vm.eval_set(lambda x: x[0][x[1]])


def call(vm: VM, ops):
    args = vm.slice(ops[0]) if ops[0] > 0 else ()
    vm.eval_set(lambda x: x(*args))


def mk_array(vm: VM, ops):
    vm.eval_set(lambda x: _Window['Array'](x))
