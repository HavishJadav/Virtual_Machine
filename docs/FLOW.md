# VM & JVM Flow

1. **Custom VM**
   - Write assembly (`example.vmasm`)
   - Assemble → bytecode:  
     `python -m vm.cli asm examples/example.vmasm out.vmc`
   - Disassemble back:  
     `python -m vm.cli dis out.vmc out.vmasm`
   - Run:  
     `python -m vm.cli run out.vmc result.txt`
   - Segments: CODE, DATA, STACK, CONST (inline), HEAP (placeholder)

2. **Java Bytecode**
   - Compile `Sample.java` with `javac`
   - Inspect bytecode with `javap -c -v Sample`
   - Optionally compare with `krakatau disassemble`
