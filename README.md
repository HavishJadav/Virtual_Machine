# TAC to RISC-V Assembly Translator

## Overview

This project is a compiler backend module implemented in C++. It reads **Three-Address Code (TAC)**—an intermediate representation—and translates it into executable **RISC-V Assembly**. The system handles basic optimizations, control flow graph construction (basic blocks), register allocation, and stack spilling.

The translator is designed to take a flat TAC file and produce a `.asm` file located in an `outputs/` directory.

## Project Structure

Based on the source files, the logical structure includes:

  * **`main.cpp`**: Entry point. Handles file I/O and creates the output directory.
  * **`CodeOptimizer.cpp`**: Pre-processes raw code, removes redundancy, and structures the linear code into **Basic Blocks**.
  * **`CodeGenerator.cpp`**: The core translation engine. Maps TAC operations to RISC-V instructions, manages the symbol table, handles string literals, and performs register allocation.

## Features

  * **Target Architecture**: RISC-V (32-bit integer operations inferred).
  * **Register Allocation**:
      * Uses a pool of registers: `x5-x7`, `x28-x31`.
      * Implements a **Round-Robin Spilling** strategy when registers are exhausted.
      * Manages variable descriptors (`regDesc`) and address descriptors (`addrDesc`) for stack storage.
  * **Control Flow**: Supports `if/else` branching, `GOTO`, labels, and `return` statements.
  * **Data Types**: Handles integers (`INT`) and string literals (`STR`).
  * **String Handling**: Automatically moves string literals to the `.data` segment and calculates offsets for printing.

## Prerequisites

  * A C++ Compiler supporting **C++17** or later (required for `<filesystem>` support in `main.cpp`).
  * A RISC-V simulator (like RARS or Spike) to run the generated assembly.

## Build Instructions

Assuming the following directory structure:

```text
.
├── include/
│   ├── CodeGenerator.hpp
│   └── CodeOptimizer.hpp
├── src/
│   ├── CodeGenerator.cpp
│   └── CodeOptimizer.cpp
└── main.cpp
```

Compile the project using `g++`:

```bash
g++ -std=c++17 main.cpp src/CodeGenerator.cpp src/CodeOptimizer.cpp -o tac_vm_translator
```

## Usage

Run the executable with the path to your input TAC file as the argument:

```bash
./tac_vm_translator <path-to-tac-file>
```

**Example:**

```bash
./tac_vm_translator tests/program.tac
```

This will generate `outputs/program.asm`.

## Supported TAC Syntax

The parser uses Regex to interpret lines. Ensure your input follows these formats:

### 1\. Declarations & Assignments

  * **Declaration:** `- TYPE name` (e.g., `- INT x`) (Note: These are largely filtered out during emission but recognized).
  * **Assignment (Int):** `dst = src TYPE` (e.g., `x = 5 INT` or `x = y INT`).
  * **Assignment (String):** `dst = "string_value" STR`.

### 2\. Arithmetic Operations

Format: `dst = operand1 OP operand2 TYPE`

  * Supported Operators: `+`, `-`, `*`, `/`, `%`, `&`, `|`, `^`, `<<`, `>>`.
  * Example: `t1 = a + b INT`

### 3\. Control Flow

  * **Labels:** `label_name:`.
  * **Unconditional Jump:** `GOTO label_name`.
  * **Conditional Jump:**
      * `if lhs OP rhs GOTO label_true else GOTO label_false`
      * Supported comparisons: `<`, `>`, `<=`, `>=`, `==`, `!=`.
  * **Return:** `return value TYPE`.

### 4\. I/O

  * **Print:** `print variable STR` (Uses RISC-V `ecall` with logic for string addressing).

## Limitations

  * **Hardcoded Memory:** The `.data` segment base address is hardcoded to `0x10010`.
  * **Optimization:** "Optimization" is currently limited to basic block construction and removing consecutive returns; it does not perform dead code elimination or constant folding.
  * **Register Spilling:** Stack offsets increase indefinitely (`spOffset+=4`) and are not reclaimed when variables go out of scope.

## Output Example

The tool automatically separates code and data:

```assembly
.section
 .data
msg: 
.asciz "Hello World"

.section
.text
    # Generated instructions
    lui x5,0x10010
    addi x5,x5,0x0
    ...
```
