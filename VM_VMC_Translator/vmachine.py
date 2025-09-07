
import struct
from dataclasses import dataclass
from typing import List
from .opcodes import OPCODES

@dataclass
class VM:
    code: bytes
    data_size: int = 64

    def __post_init__(self):
        self.pc = 0
        self.stack: List[int] = []
        self.data: List[int] = [0] * self.data_size
        self.call_frames: List[int] = []
        self.running = True

    def step(self):
        op = self.code[self.pc]; self.pc += 1
        if op == OPCODES['NOP']:
            pass
        elif op == OPCODES['PUSH']:
            val = struct.unpack_from('<i', self.code, self.pc)[0]; self.pc += 4
            self.stack.append(val)
        elif op == OPCODES['POP']:
            self.stack.pop()
        elif op == OPCODES['ADD']:
            b = self.stack.pop(); a = self.stack.pop(); self.stack.append(a + b)
        elif op == OPCODES['SUB']:
            b = self.stack.pop(); a = self.stack.pop(); self.stack.append(a - b)
        elif op == OPCODES['MUL']:
            b = self.stack.pop(); a = self.stack.pop(); self.stack.append(a * b)
        elif op == OPCODES['DIV']:
            b = self.stack.pop(); a = self.stack.pop(); self.stack.append(a // b)
        elif op == OPCODES['LOAD']:
            idx = struct.unpack_from('<H', self.code, self.pc)[0]; self.pc += 2
            self.stack.append(self.data[idx])
        elif op == OPCODES['STORE']:
            idx = struct.unpack_from('<H', self.code, self.pc)[0]; self.pc += 2
            self.data[idx] = self.stack.pop()
        elif op == OPCODES['JMP']:
            addr = struct.unpack_from('<H', self.code, self.pc)[0]; self.pc = addr
        elif op == OPCODES['JZ']:
            addr = struct.unpack_from('<H', self.code, self.pc)[0]; self.pc += 2
            v = self.stack.pop()
            if v == 0:
                self.pc = addr
        elif op == OPCODES['CALL']:
            addr = struct.unpack_from('<H', self.code, self.pc)[0]; self.pc += 2
            self.call_frames.append(self.pc)
            self.pc = addr
        elif op == OPCODES['RET']:
            self.pc = self.call_frames.pop()
        elif op == OPCODES['HALT']:
            self.running = False
        elif op == OPCODES['DUP']:
            self.stack.append(self.stack[-1])
        elif op == OPCODES['SWAP']:
            self.stack[-1], self.stack[-2] = self.stack[-2], self.stack[-1]
        else:
            raise RuntimeError(f"Unknown opcode: {op}")

    def run(self, max_steps=100000):
        steps = 0
        while self.running and steps < max_steps:
            self.step()
            steps += 1
        return self.stack
