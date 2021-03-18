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
    0: InstrConfig('prop:get', func=vm.get_prop),
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
    12: InstrConfig('combine:stack:pop', func=vm.combine_stack),
    14: InstrConfig('swap:top,v+1', func=vm.swap),
    15: InstrConfig('dup', func=vm.dup),
    16: InstrConfig('mk:array', func=vm.mk_array),
    18: InstrConfig('create_instance', stack_props={'dynamic': True, 'absolute': False}, func=vm.call),
    21: InstrConfig('~'),
    22: InstrConfig('&'),
    23: InstrConfig('jmp', cdd({0: CASTS['label']})),
    24: InstrConfig('pop', func=vm.pop),
    25: InstrConfig('apply:func', stack_props={'dynamic': True, 'absolute': False}),
    26: InstrConfig('window[]', func=vm.window_prop_getter),
    27: InstrConfig('add', func=vm.add),
    28: InstrConfig('prop:del'),
    29: InstrConfig('sub', func=vm.sub),
    30: InstrConfig('prop:set:nopop', func=vm.prop_set_nopop),
    31: InstrConfig('push:v', func=vm.push),
    32: InstrConfig('^', func=vm.xor),
    33: InstrConfig('|'),
    34: InstrConfig('div'),
    35: InstrConfig('ge'),
    36: InstrConfig('create_instance:prop', stack_props={'dynamic': True, 'absolute': False}),
    37: InstrConfig('not'),
    39: InstrConfig('>>>'),
    40: InstrConfig('push:undefined', func=vm.push_undefined),
    41: InstrConfig('push:true', func=vm.push_true),
    42: InstrConfig('push:false', func=vm.push_false),
    43: InstrConfig('shr', func=vm.shr),
    44: InstrConfig('push:vm', cdd({0: CASTS['label']})),
    45: InstrConfig('push:""', func=vm.push_empty_str),
    46: InstrConfig('mul', func=vm.mul),
    47: InstrConfig('set:g=null'),
    50: InstrConfig('dup:n', func=vm.dup_n),
    51: InstrConfig('prop:set:poppos', func=vm.prop_set_pop_valpos),
    52: InstrConfig('jnz', cdd({0: CASTS['label']})),
    53: InstrConfig('throw'),
    54: InstrConfig('ret:true'),
    55: InstrConfig('mod'),
    56: InstrConfig('brk', stack_props={'dynamic': True, 'absolute': True}),
    58: InstrConfig('==='),
    60: InstrConfig('prop:set:popprop', func=vm.prop_set_pop_propval),
    61: InstrConfig('gt'),
    62: InstrConfig('init_stack:list'),
    63: InstrConfig('combine:prop:pop', func=vm.combine_prop),
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
PROP_SET_NOPOP = 30