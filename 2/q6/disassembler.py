from __future__ import annotations
from typing import Dict, List, Callable, Any
from dataclasses import dataclass, field
from collections import defaultdict
import json


import instr_def
import instr_config


def id_func(x):
    return x


@dataclass
class Instr:
    code: int
    name: str
    op_count: int
    op_casts: Dict[int, Callable[[int], Any]] = field(default_factory=lambda: defaultdict(lambda: id_func))
    ops_cast: Callable[[List[int]], Any] = id_func
    var_operands: bool = False
    calc_var_op_count: Callable[[List[int]], int] = lambda x: len(x)
    stack_op: int = 0

    def __post_init__(self):
        if not self.name:
            self.name = f'instr{self.code}'

    def __str__(self):
        name_len = max(instr_config.MAX_INSTR_DESC_LEN, 7)
        return f'INSTR{self.code:<2}<{self.name:>{name_len}}>'


@dataclass
class OptInstr(Instr):
    def __str__(self):
        name_len = max(instr_config.MAX_INSTR_DESC_LEN, 7)
        return f'OPTIN{self.code:<2}<{self.name:>{name_len}}>'


@dataclass
class Code:
    pos: int
    instr: Instr
    operands: List[int] = field(default_factory=list)

    def __str__(self):
        if len(self.operands):
            return f'{self.pos:>6}: {str(self.instr)}    {self.str_operands()}'
        else:
            return f'{self.pos:>6}: {str(self.instr)}'

    def operand_val(self, i):
        operand = self.operands[i]
        return self.instr.op_casts[i](operand)

    def operands_val(self):
        return self.instr.ops_cast(self.operands)

    def str_operands(self):
        return ", ".join(map(lambda i: self.str_operand(i), range(len(self.operands))))

    def str_operand(self, i):
        operand = self.operands[i]
        r = self.instr.op_casts[i](operand)
        if r != operand:
            return f'{operand!s}({r!s})'
        else:
            return str(operand)


@dataclass
class OptCode(Code):
    origin_codes: List[Code] = field(default_factory=list)

    def append(self, code: Code):
        self.origin_codes.append(code)
        self.operands += code.operands

    def str_operands(self):
        r = self.instr.ops_cast(self.operands)
        if r != self.operands:
            return f'{super().str_operands()}({r!s})'
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
        if op_count >= 3:
            if has_var_operands_instr:
                raise Exception("duplicate instr??")
            op_count = 3
            has_var_operands_instr = True
            instrs[code] = Instr(code, code_config.alias, op_count, code_config.op_labels, var_operands=True,
                                 calc_var_op_count=_calc_var_op_count)
        elif instr_source == "nop":
            instrs[code] = Instr(code, 'nop', op_count)
        else:
            instrs[code] = Instr(code, code_config.alias, op_count, code_config.op_labels)
    return instrs


def disassembler(program: List[int], instrs: Dict[int, Instr]) -> ReadableProgram[Code]:
    length = len(program)
    i = 0
    codes = ReadableProgram()
    while i < length:
        pos = i
        code = program[i]
        instr = instrs.get(code, None)
        if not instr:
            # i += 1
            # continue
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


if __name__ == '__main__':
    instrs = load_instrs(instr_def)
    with open('program.json') as fin, open('program.vmasm.txt', 'w+') as fout:
        program = json.load(fin)
        print('program length:', len(program))
        codes = disassembler(program, instrs)
        fout.write(str(codes))
