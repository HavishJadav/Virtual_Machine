# vm_memory_segments.py

class CodeSegment:
    def __init__(self, bytecode: bytes):
        self._bytecode = bytecode

    @property
    def bytecode(self) -> bytes:
        return self._bytecode

    def __setattr__(self, key, value):
        if key == "_bytecode" and hasattr(self, "_bytecode"):
            raise AttributeError("CodeSegment bytecode is read-only and cannot be modified.")
        super().__setattr__(key, value)

    def __repr__(self):
        return f"<CodeSegment length={len(self._bytecode)} bytes>"

class GlobalData:
    def __init__(self):
        self._globals = {}

    def set(self, name: str, value):
        self._globals[name] = value

    def get(self, name: str):
        return self._globals.get(name)

    def __repr__(self):
        return f"<GlobalData variables={list(self._globals.keys())}>"

class ConstantPool:
    def __init__(self, constants):
        self._constants = tuple(constants)

    def get_constant(self, index: int):
        if index < 0 or index >= len(self._constants):
            raise IndexError("ConstantPool index out of range")
        return self._constants[index]

    def __repr__(self):
        return f"<ConstantPool size={len(self._constants)}>"

class Stack:
    def __init__(self, max_size: int):
        self._max_size = max_size
        self._stack = []

    def push(self, item):
        if len(self._stack) >= self._max_size:
            raise MemoryError("Stack overflow")
        self._stack.append(item)

    def pop(self):
        if not self._stack:
            raise IndexError("Pop from an empty stack")
        return self._stack.pop()

    def peek(self):
        if not self._stack:
            raise IndexError("Peek into an empty stack")
        return self._stack[-1]

    def __len__(self):
        return len(self._stack)

    def __repr__(self):
        return f"<Stack size={len(self)}/{self._max_size}>"

if __name__ == "__main__":
    code = CodeSegment(b"\x01\x02\x03\x04")
    print(code)
    print("Bytecode:", code.bytecode)

    globals_area = GlobalData()
    globals_area.set("counter", 10)
    globals_area.set("flag", True)
    print(globals_area)
    print("Global 'counter':", globals_area.get("counter"))

    const_pool = ConstantPool(["Hello", 42, 3.14])
    print(const_pool)
    print("Constant at index 1:", const_pool.get_constant(1))

    stack = Stack(max_size=3)
    stack.push(100)
    stack.push(200)
    print(stack)
    print("Popped from stack:", stack.pop())
    print(stack)

    try:
        code._bytecode = b"\x00"
    except AttributeError as e:
        print("Error:", e)

    try:
        print(const_pool.get_constant(10))
    except IndexError as e:
        print("Error:", e)