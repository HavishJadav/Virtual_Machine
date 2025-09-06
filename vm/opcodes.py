"""
Opcode definitions and mappings for the SimpleVM.
"""

OPCODES = {
    'NOP':   0x00,
    'PUSH':  0x01,
    'POP':   0x02,
    'ADD':   0x03,
    'SUB':   0x04,
    'MUL':   0x05,
    'DIV':   0x06,
    'LOAD':  0x07,
    'STORE': 0x08,
    'JMP':   0x09,
    'JZ':    0x0A,
    'CALL':  0x0B,
    'RET':   0x0C,
    'HALT':  0x0D,
    'DUP':   0x0E,
    'SWAP':  0x0F,
}

OPNAMES = {v: k for k, v in OPCODES.items()}
