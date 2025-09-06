"""
Assembler: convert VM assembly (text) into bytecode.
Supports labels, whitespace, and comments.
"""
import struct
from typing import List, Tuple, Dict, Optional
from .opcodes import OPCODES

def _clean(line: str) -> str:
    i = line.find('//')
    if i >= 0:
        line = line[:i]
    return line.strip()

def parse_assembly(lines: List[str]) -> Tuple[List[Tuple[str, Optional[str]]], Dict[str, int]]:
    pc = 0
    labels: Dict[str, int] = {}
    tokens: List[Tuple[str, Optional[str]]] = []

    for line in lines:
        cl = _clean(line)
        if not cl:
            continue
        if cl.endswith(':'):
            lbl = cl[:-1].strip()
            labels[lbl] = pc
            continue

        parts = cl.split()
        mnem = parts[0].upper()
        op = parts[1] if len(parts) > 1 else None

        if mnem == 'PUSH':
            pc += 1 + 4
        elif mnem in ('LOAD', 'STORE'):
            pc += 1 + 2
        elif mnem in ('JMP', 'JZ', 'CALL'):
            pc += 1 + 2
        elif mnem in OPCODES:
            pc += 1
        else:
            raise ValueError(f"Unknown mnemonic: {mnem}")

        tokens.append((mnem, op))

    return tokens, labels

def assemble(lines: List[str]) -> Tuple[bytes, Dict[str, int]]:
    tokens, labels = parse_assembly(lines)
    bc = bytearray()

    for mnem, op in tokens:
        code = OPCODES[mnem]
        bc.append(code)

        if mnem == 'PUSH':
            val = int(op, 0)
            bc.extend(struct.pack('<i', val))
        elif mnem in ('LOAD', 'STORE'):
            idx = int(op, 0)
            bc.extend(struct.pack('<H', idx))
        elif mnem in ('JMP', 'JZ', 'CALL'):
            if op.isidentifier() and op in labels:
                addr = labels[op]
            else:
                addr = int(op, 0)
            bc.extend(struct.pack('<H', addr))

    return bytes(bc), labels
