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

class Heap:
    def __init__(self, max_size: int):
        self._max_size = max_size
        self._memory = {}
        self._next_addr = 0

    def alloc(self, obj) -> int:
        if len(self._memory) >= self._max_size:
            raise MemoryError("Heap overflow")
        
        addr = self._next_addr
        self._memory[addr] = obj
        self._next_addr += 1
        return addr

    def free(self, address: int):
        if address not in self._memory:
            raise ValueError(f"Invalid heap address: {address}")
        del self._memory[address]

    def get(self, address: int):
        if address not in self._memory:
            raise ValueError(f"Invalid heap address: {address}")
        return self._memory[address]

    def __repr__(self):
        return f"<Heap size={len(self._memory)}/{self._max_size}>"

if __name__ == "__main__":
    code = CodeSegment(b"\x01\x02\x03\x04")
    print(code)
    print("Bytecode:", code.bytecode)
    print("-" * 20)

    globals_area = GlobalData()
    globals_area.set("counter", 10)
    globals_area.set("flag", True)
    print(globals_area)
    print("Global 'counter':", globals_area.get("counter"))
    print("-" * 20)

    const_pool = ConstantPool(["Hello", 42, 3.14])
    print(const_pool)
    print("Constant at index 1:", const_pool.get_constant(1))
    print("-" * 20)

    stack = Stack(max_size=3)
    stack.push(100)
    stack.push(200)
    print(stack)
    print("Popped from stack:", stack.pop())
    print(stack)
    print("-" * 20)

    print("Heap Tests:")
    heap = Heap(max_size=5)
    print(heap)
    
    addr1 = heap.alloc({"type": "user", "id": 101})
    addr2 = heap.alloc([1, 2, 3, 4, 5])
    print(f"Allocated object 1 at address: {addr1}")
    print(f"Allocated object 2 at address: {addr2}")
    print(heap)
    
    retrieved_obj = heap.get(addr1)
    print(f"Retrieved object from address {addr1}: {retrieved_obj}")
    
    heap.free(addr1)
    print(f"Freed memory at address: {addr1}")
    print(heap)
    print("-" * 20)
    
    print("Error Handling:")
    try:
        code.bytecode = b"\x00"
    except AttributeError as e:
        print("Error:", e)

    try:
        print(const_pool.get_constant(10))
    except IndexError as e:
        print("Error:", e)
        
    try:
        stack.push(300)
        stack.push(400)
    except MemoryError as e:
        print("Error:", e)

    try:
        stack.pop()
        stack.pop()
    except IndexError as e:
        print("Error:", e)
        
    try:
        heap.get(addr1)
    except ValueError as e:
        print("Error:", e)
