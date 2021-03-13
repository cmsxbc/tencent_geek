from typing import Dict, List, Callable, Any
from dataclasses import dataclass, field
from collections import defaultdict

import init_program


CASTS = {
    'chr': chr,
    'label': lambda x: f'pos:{x}'
}

MAX_INSTR_DESC_LEN = 32


@dataclass
class Instr:
    code: int
    name: str
    op: int
    op_casts: Dict[int, Callable[[int], Any]]
    ops_cast: Callable[[List[int]], Any] = lambda x: x

    def __post_init__(self):
        if not self.name:
            self.name = f'instr{self.code}'

    def __str__(self):
        return f'INSTR{self.code:<2}<{self.name:>{MAX_INSTR_DESC_LEN}}>'


@dataclass
class OptInstr(Instr):
    op_casts: Dict[int, Callable[[int], Any]] = field(default_factory=lambda: defaultdict(lambda: lambda x: x))
    def __str__(self):
        return f'OPTIN{self.code:<2}<{self.name:>{MAX_INSTR_DESC_LEN}}>'


@dataclass
class Code:
    pos: int
    instr: Instr
    operands: List[int] = field(default_factory=list)

    def __str__(self):
        if len(self.operands):
            return f'{self.pos:>4}: {str(self.instr)}    {self._str_operands()}'
        else:
            return f'{self.pos:>4}: {str(self.instr)}'

    def _str_operands(self):
        return ", ".join(map(lambda i: self._str_operand(i), range(len(self.operands))))

    def _str_operand(self, i):
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

    def _str_operands(self):
        r = self.instr.ops_cast(self.operands)
        if r != self.operands:
            return f'{super()._str_operands()}({r!s})'
        else:
            return super()._str_operands()


@dataclass
class RecOptCode(OptCode):
    def _str_operands(self):
        return self.instr.ops_cast(self.operands)


@dataclass
class ReadableProgram(list):
    def __str__(self):
        return '\n'.join(map(str, self))


def load_instrs(instrs_filepath: str) -> Dict[int, Instr]:
    instrs = {}
    with open(instrs_filepath) as fin:
        for line in fin.readlines():
            if not line.strip():
                continue
            instr_desc, *alias = line.split(';')
            instr, *ops = instr_desc.strip().split(' ')
            code = int(instr.replace('instr', '').strip())
            # op = len(list(filter(lambda x: x.strip() == '<op>', ops)))
            op_count = 0
            op_casts = defaultdict(lambda: lambda x: x)
            for op in ops:
                op = op.strip()
                if not op:
                    continue
                if op[:3] != '<op':
                    continue
                if op[3] == ',':
                    op_casts[op_count] = CASTS[op[4:-1]]
                op_count += 1
            name = alias[0].strip() if len(alias) else ''
            instrs[code] = Instr(code, name, op_count, op_casts)
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
            raise Exception(f"{i} => {program[i]} has no instr")
        i += 1
        if instr.code == 3:
            _, n, t = program[i: i+instr.op]
            dynamic_op_len = 2 * n + t
            total_op_len = 3 + 2 * n + t
            codes.append(Code(pos, instr, program[i: i + total_op_len]))
            i += total_op_len
        elif not instr.op:
            codes.append(Code(pos, instr))
        else:
            codes.append(Code(pos, instr, program[i: i+instr.op]))
            i += instr.op
    return codes


def optimize(codes: ReadableProgram[Code], level: int = -1) -> ReadableProgram[Code]:
    length = len(codes)
    i = 0
    optimized_codes = ReadableProgram()
    state = 'N'
    new_code = None

    def _just_append(i):
        optimized_codes.append(codes[i])
        return i + 1

    def _merge_opt16(i):
        for _codes in ((16,45,55,52), (16,45,16,52)):
            for _code, c in zip(codes[i:i+4], _codes):
                if _code.instr.code != c:
                    break
            else:
                new_code = RecOptCode(
                    codes[i].pos,
                    OptInstr(
                        99,
                        'PUSH_BIGINT', -1,
                        ops_cast=lambda _: codes[i+2]._str_operands()
                    )
                )
                for _code in codes[i:i+4]:
                    new_code.append(_code)
                optimized_codes.append(new_code)
                return i + 4
        else:
            return _just_append(i)

    while i < length:
        code = codes[i]
        if state == 'N':
            if code.instr.code == 16:
                if isinstance(code.instr, OptInstr):
                    i = _merge_opt16(i)
                    continue
                state = 'APPEND_STR'
                new_code = OptCode(
                    code.pos,
                    OptInstr(
                        code.instr.code,
                        'PUSH_STR', -1,
                        ops_cast=lambda xs: ''.join(map(chr, xs))
                    )
                )
                i += 1
            else:
                i = _just_append(i)
        elif state == 'APPEND_STR':
            if code.instr.code != 68:
                optimized_codes.append(new_code)
                state = 'N'
            else:
                new_code.append(code)
                i += 1
        else:
            raise Exception("your are poor!")

    return optimized_codes if len(optimized_codes) == len(codes) or level == 1 else optimize(optimized_codes, level - 1)


if __name__ == '__main__':
    codes = disassembler(
        init_program.program,
        load_instrs('instrs-operand.txt')
    )
    with open('disassemble_init_program.txt', 'w+') as f:
        f.write(str(codes))

    with open('optimize_disassemble_init_program.txt', 'w+') as f:
        f.write(str(optimize(codes, level=-1)))