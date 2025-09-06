import struct
from typing import List, Dict
from .opcodes import OPNAMES

def disassemble(bc: bytes, labels: Dict[str, int] = None) -> List[str]:
    labels = labels or {}
    out = []
    i = 0
    addr_to_label = {v: k for k, v in labels.items()}

    while i < len(bc):
        addr = i
        if addr in addr_to_label:
            out.append(f"{addr_to_label[addr]}:")

        op = bc[i]; i += 1
        name = OPNAMES.get(op, f'OP_{op:02X}')

        if name == 'PUSH':
            (val,) = struct.unpack_from('<i', bc, i); i += 4
            out.append(f'{name} {val}')
        elif name in ('LOAD', 'STORE', 'JMP', 'JZ', 'CALL'):
            (arg,) = struct.unpack_from('<H', bc, i); i += 2
            target = addr_to_label.get(arg, str(arg))
            out.append(f'{name} {target}')
        else:
            out.append(name)

    return out
