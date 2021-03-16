from __future__ import annotations
from typing import Dict, List, Callable, Any, Union, Optional
from dataclasses import dataclass, field, InitVar
from collections import defaultdict
import json
import base64


import instr_def
import instr_config
import vm


def id_func(x):
    return x


@dataclass
class StackOp:
    instr_source: InitVar[str] = ''
    config: InitVar[Dict[str, Union[int, bool]]] = field(default={})
    push: int = 0
    pop: int = 0
    delta: int = field(default=0, init=False)
    dynamic: bool = False
    absolute: bool = False

    def __post_init__(self, instr_source, config):
        if instr_source:
            r = {
                'push': instr_source.count(f'{instr_def.stack_var}.push('),
                'pop': -instr_source.count(f'{instr_def.stack_var}.pop('),
            }
        else:
            r = {}
        r.update(config)
        for k, v in r.items():
            setattr(self, k, v)
        if not self.dynamic and self.absolute:
            raise Exception("you sucks!!")
        self.delta = self.push + self.pop

    def __str__(self):
        return f'{self.dynamic:1d},{self.absolute:1d};{self.push:+1},{self.pop:+1},{self.delta:+1}'

    def calc_delta(self, operands: List[int]) -> int:
        if self.dynamic:
            if self.absolute:
                return operands[0]
            return self.delta - operands[0]
        else:
            return self.delta

    def str_calc_delta(self, operands: List[int]) -> str:
        val = self.calc_delta(operands)
        width = 4
        if self.absolute:
            return f'={val}'.rjust(width)
        else:
            return f'{val:>+{width}d}'


@dataclass
class Instr:
    code: int
    name: str
    op_count: int
    stack_op: StackOp
    op_casts: Dict[int, Callable[[int], Any]] = field(default_factory=lambda: defaultdict(lambda: id_func))
    ops_cast: Callable[[List[int]], Any] = id_func
    var_operands: bool = False
    calc_var_op_count: Callable[[List[int]], int] = lambda x: len(x)
    func: vm.VmFunc = vm.default_vm_func

    def __post_init__(self):
        if not self.name:
            self.name = f'instr{self.code}'

    def __str__(self):
        name_len = max(instr_config.MAX_INSTR_DESC_LEN, 7)
        class_name = self.__class__.__name__
        return f'{class_name}{self.code:<2}[{self.stack_op!s}]<{self.name:>{name_len}}>'

    def stack_delta(self, operands: List[int]) -> int:
        return self.stack_op.calc_delta(operands)

    def str_stack_delta(self, operands: List[int]) -> str:
        return self.stack_op.str_calc_delta(operands)

    def run(self, vm: vm.VM, ops: List[int]):
        return self.func(vm, ops)


@dataclass
class OptIt(Instr):
    pass


@dataclass
class Code:
    pos: int
    instr: Instr
    operands: List[int] = field(default_factory=list)

    def __str__(self):
        if len(self.operands):
            return f'{self.pos:>6}: {str(self.instr)}[{self.str_stack_delta()}]    {self.str_operands()}'
        else:
            return f'{self.pos:>6}: {str(self.instr)}[{self.str_stack_delta()}]'

    def operand_val(self, i):
        operand = self.operands[i]
        return self.instr.op_casts[i](operand)

    def operands_val(self):
        return self.instr.ops_cast(self.operands)

    def stack_delta(self) -> int:
        return self.instr.stack_delta(self.operands)

    def str_stack_delta(self) -> str:
        return self.instr.str_stack_delta(self.operands)

    def str_operands(self):
        return ", ".join(map(lambda i: self.str_operand(i), range(len(self.operands))))

    def str_operand(self, i):
        operand = self.operands[i]
        r = self.instr.op_casts[i](operand)
        if r != operand:
            return f'{operand!s}({r!s})'
        else:
            return str(operand)

    def run(self, vm: vm.VM):
        return self.instr.run(vm, self.operands)


@dataclass
class OptCode(Code):
    origin_codes: List[Code] = field(default_factory=list)

    def append(self, code: Code):
        self.origin_codes.append(code)
        self.operands += code.operands


@dataclass
class AutoOptCode(OptCode):

    def str_operands(self):
        r = self.instr.ops_cast(self.operands)
        if r != self.operands:
            return f'{super().str_operands()}({r!s})'
        else:
            return super().str_operands()


@dataclass
class HumanOptCode(OptCode):
    human_ops: List[Any] = field(default_factory=list)

    def str_operands(self):
        if len(self.human_ops):
            return ','.join(map(repr, self.human_ops))
        else:
            return super().str_operands()


@dataclass
class RecOptCode(OptCode):
    def str_operands(self):
        return self.instr.ops_cast(self.operands)


@dataclass
class ReadableProgram(list):
    def __str__(self):
        return '\n'.join(map(str, self))


@dataclass
class InstrDef:
    instr_sources: Dict[int, str]
    pos_var: str
    stack_var: str
    program_var: str


def load_instrs(config: InstrDef) -> Dict[int, Instr]:
    instrs = {}
    has_var_operands_instr = False

    def _count_operand(s: str):
        return s.count(f'{config.pos_var}++')

    def _calc_var_op_count(codes: List[int]) -> int:
        _, n, t = codes[:3]
        return 2 * n + t

    for code, instr_source in config.instr_sources.items():
        op_count = _count_operand(instr_source)
        code_config = instr_config.config[code]
        stack_op = StackOp(instr_source, code_config.stack_props)
        if op_count >= 3:
            if has_var_operands_instr:
                raise Exception("duplicate instr??")
            op_count = 3
            has_var_operands_instr = True
            instrs[code] = Instr(code, code_config.alias, op_count, stack_op, code_config.op_labels, var_operands=True,
                                 calc_var_op_count=_calc_var_op_count)
        elif instr_source == "nop":
            instrs[code] = Instr(code, 'nop', op_count, stack_op)
        else:
            instrs[code] = Instr(code, code_config.alias, op_count, stack_op, code_config.op_labels)
        if code_config.func:
            instrs[code].func = code_config.func
    return instrs


def disassembler(program: List[int], instrs: Dict[int, Instr], offset: int = 0) -> ReadableProgram[Code]:
    length = len(program)
    i = offset
    codes = ReadableProgram()
    while i < length:
        pos = i
        code = program[i]
        instr = instrs.get(code, None)
        if not instr:
            # i += 1
            # continue
            # print(codes)
            raise Exception(f"{i} => {program[i]} has no instr")
        i += 1
        if instr.var_operands:
            var_op_len = instr.calc_var_op_count(program[i: i+instr.op_count])
            total_op_len = var_op_len + instr.op_count
            codes.append(Code(pos, instr, program[i: i + total_op_len]))
            i += total_op_len
        elif not instr.op_count:
            codes.append(Code(pos, instr))
        else:
            codes.append(Code(pos, instr, program[i: i+instr.op_count]))
            i += instr.op_count
    return codes


def run_segment(segment: List[Code], v: Optional[vm.VM] = None) -> vm.VM:
    if v is None:
        v = vm.VM()
    for code_ in segment:
        code_.run(v)
    return v


def run_forever(v: vm.VM, start: int, program: List[Code]) -> int:
    try:
        while True:
            program[start].run(v)
            start += 1
    except NotImplementedError:
        print('\t\tnot implement', program[start])
        return start


OPT_INSTR_CODES = {
    'str': 0
}


def optimize_str(program: ReadableProgram[Code]) -> (ReadableProgram[Code], str):
    length = len(program)
    i = 0
    optimized_program = ReadableProgram()
    longest_str = ''

    def mk_push_str(segment: List[Code]):
        nonlocal longest_str
        operands = [op for c in segment for op in c.operands]
        v = run_segment(segment)
        human_ops = list(v)

        if v.depth() == 1:
            name = 'push:str'
        else:
            name = 'push:str_se'

        def _vm_func(vm_: vm.VM, ops: List[int]):
            for op in human_ops:
                vm_.push(op)

        instr = OptIt(OPT_INSTR_CODES['str'], name, len(operands) + len(segment) - 1, StackOp(push=v.depth()), func=_vm_func)
        if len(longest_str) < len(human_ops[0]):
            longest_str = human_ops[0]
        return HumanOptCode(segment[0].pos, instr, operands, segment, human_ops)

    def run_until_string_end(start: int, v: Optional[vm.VM] = None) -> (int, int):
        if v is None:
            v = vm.VM()
        start_depth = v.depth()
        program[start].run(v)
        chr_pos = start
        start += 1
        try:
            while v.depth() - start_depth < 4:
                _code = program[start]
                _code.run(v)
                if _code.instr.code == instr_config.CHR_INSTR:
                    chr_pos = start
                start += 1
        except NotImplementedError:
            # print('\t\tnot implement', program[start])
            pass
        except Exception as e:
            # print(e)
            pass
        return chr_pos + 1, start

    while i < length:
        code = program[i]
        new_code = code
        if code.instr.code == instr_config.START_STRING_INSTR:
            new_i, _ = run_until_string_end(i)
            if new_i == i:
                raise Exception("you sb!")
            else:
                range_codes: List[Code] = program[i:new_i]
                new_code = mk_push_str(range_codes)
                # print(new_code)
            i = new_i
        else:
            i += 1
        optimized_program.append(new_code)

    return optimized_program, longest_str


def optimize_array(program: ReadableProgram[Code]) -> (ReadableProgram[Code], List[int]):
    length = len(program)
    i = 0
    optimized_program = ReadableProgram()
    longest_array = []

    def mk_push_array(segment: List[Code], v: Optional[vm.VM] = None):
        nonlocal longest_array
        if v is None:
            v = vm.VM()
        res = {}
        v.push(res)
        for c in segment:
            c.run(v)
        longest_array = list(res.values())
        return

    def run_until_array_end(start: int, v: Optional[vm.VM] = None) -> int:
        if v is None:
            v = vm.VM()
        start_depth = v.depth()
        program[start].run(v)
        last_prop_set = start
        start += 1
        try:
            while v.depth() - start_depth < 4:
                _code = program[start]
                _code.run(v)
                if _code.instr.code == instr_config.PROP_SET_NOPOP:
                    last_prop_set = start
                start += 1
        except NotImplementedError:
            v.pretty_print()
            print('\t\tnot implement', program[start])
            raise
        except Exception as e:
            print(e)
            v.pretty_print()
            pass
        return last_prop_set

    while i < length:
        code = program[i]
        new_code = code
        if code.instr.code == OPT_INSTR_CODES['str'] and isinstance(code.instr, OptIt) and code.human_ops[0] == 'Array':
            new_i = run_until_array_end(i)
            if new_i == i:
                i += 1
                pass
                # raise Exception("you sb!")
            else:
                range_codes: List[Code] = program[i:new_i]
                new_code = mk_push_array(range_codes)
                i = new_i
        else:
            i += 1
        optimized_program.append(new_code)

    return optimized_program, longest_array


def gen_real_program(source: str, patch_array: List[int]) -> List[int]:
    program = []
    patches = {
        patch_array[i]: patch_array[i+1] for i in range(0, len(patch_array), 2)
    }
    print(patches)
    for c in base64.b64decode(source):
        v = patches.get(len(program), None)
        if v is not None:
            program.append(v)
        program.append(c)
    print(program)
    print(len(program))
    return program


if __name__ == '__main__':
    import os
    instrs = load_instrs(instr_def)
    offsets = (0, 48, 103, 17726, 18203, 19590, 20867, 20942)
    with open('program.json') as fin:
        program = json.load(fin)
        print('program length:', len(program))
        for offset in offsets:
            codes = disassembler(program, instrs, offset)
            if not os.path.isdir(f'offset_program/{offset}'):
                os.makedirs(f'offset_program/{offset}')
            with open(f'offset_program/{offset}/program.vmasm.txt', 'w+') as fout:
                fout.write(str(codes))
            optimized_codes, longest_str = optimize_str(codes)
            with open(f'offset_program/{offset}/optimized.str.vmasm.txt', 'w+') as fout:
                fout.write(str(optimized_codes))
            with open(f'offset_program/{offset}/longest_str.txt', 'w+') as f:
                f.write(longest_str)

            optimized_codes, _ = optimize_array(optimized_codes)
            break