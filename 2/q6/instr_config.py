from dataclasses import dataclass, field
from typing import Dict, Callable, Any, Union, List, NewType
from collections import defaultdict


import vm


CASTS = {
    'chr': chr,
    'label': lambda x: f'pos:{x}'
}


def id_func(x):
    return x


def cdd(init_dict, default_factory: Any = lambda: id_func):
    return defaultdict(default_factory, init_dict.items())


@dataclass
class InstrConfig:
    alias: str = ''
    op_labels: Dict[int, Callable] = field(default_factory=lambda: defaultdict(lambda: id_func))
    stack_props: Dict[str, Union[int, bool]] = field(default_factory=dict)
    func: vm.VmFunc = vm.default_vm_func


config: Dict[int, InstrConfig] = cdd({
    0: InstrConfig('prop:get'),
    1: InstrConfig('set_stack:nopop'),
    2: InstrConfig('shl', func=vm.shl),
    3: InstrConfig('typeof'),
    4: InstrConfig('push:null'),
    5: InstrConfig('eq'),
    6: InstrConfig('set_stack:pop'),
    7: InstrConfig('combine:2', func=vm.combine),
    8: InstrConfig('mov:rsp,v', func=vm.set_top),
    10: InstrConfig('shirk_stack', stack_props={'dynamic': True, 'absolute': False}),
    11: InstrConfig('in:snd,top'),
    14: InstrConfig('swap:top,v+1'),
    15: InstrConfig('dup', func=vm.dup),
    16: InstrConfig('mklist'),
    18: InstrConfig('create_instance', stack_props={'dynamic': True, 'absolute': False}),
    21: InstrConfig('~'),
    22: InstrConfig('&'),
    23: InstrConfig('jmp', cdd({0: CASTS['label']})),
    24: InstrConfig('pop', func=vm.pop),
    25: InstrConfig('apply:func', stack_props={'dynamic': True, 'absolute': False}),
    26: InstrConfig('window[]'),
    27: InstrConfig('add', func=vm.add),
    28: InstrConfig('prop:del'),
    29: InstrConfig('sub', func=vm.sub),
    30: InstrConfig('prop:set:nopop', func=vm.prop_set_nopop),
    31: InstrConfig('push:v', func=vm.push),
    32: InstrConfig('^'),
    33: InstrConfig('|'),
    34: InstrConfig('div'),
    35: InstrConfig('ge'),
    36: InstrConfig('create_instance:prop', stack_props={'dynamic': True, 'absolute': False}),
    37: InstrConfig('not'),
    39: InstrConfig('>>>'),
    40: InstrConfig('push:undefined'),
    41: InstrConfig('push:true'),
    42: InstrConfig('push:false'),
    43: InstrConfig('shr', func=vm.shr),
    44: InstrConfig('push:vm', cdd({0: CASTS['label']})),
    45: InstrConfig('push:""', func=vm.push_empty_str),
    46: InstrConfig('mul', func=vm.mul),
    47: InstrConfig('set:g=null'),
    50: InstrConfig('dup:n'),
    52: InstrConfig('jnz', cdd({0: CASTS['label']})),
    53: InstrConfig('throw'),
    54: InstrConfig('ret:true'),
    55: InstrConfig('mod'),
    56: InstrConfig('brk', stack_props={'dynamic': True, 'absolute': True}),
    58: InstrConfig('==='),
    61: InstrConfig('gt'),
    62: InstrConfig('init_stack:list'),
    64: InstrConfig('apply:method', stack_props={'dynamic': True, 'absolute': False}),
    66: InstrConfig('chr:+top', func=vm.append_chr),
    67: InstrConfig('return:!!g')
}, default_factory=InstrConfig)


MAX_INSTR_DESC_LEN = len(max(config.values(), key=lambda x: len(x.alias)).alias)

START_STRING_INSTR = 45
CHR_INSTR = 66
CREATE_INSANCE = 18
WINDOW_PROP = 26
APPLY_FUNC = 25
