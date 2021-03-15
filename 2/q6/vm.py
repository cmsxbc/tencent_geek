from dataclasses import dataclass, field
from typing import List, Callable, Any, NewType, TypeVar


T = TypeVar('T')


@dataclass
class VM:
    stack: List = field(default_factory=list)

    def pop(self):
        return self.stack.pop()

    def slice(self, n):
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


def dup(vm: VM, ops):
    vm.dup()


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
