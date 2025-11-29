# VM_TAC_Translator (C++) - How to Run

This directory contains a C++ port of the TAC-to-RISC-V translator (previously in Python).

## Build

- Requirements: g++ with C++17, make

```bash
cd VM_TAC_Translator
make
```

This produces `tac_vm_translator`.

## Run

Pass a TAC file as input. The translator will write `outputs/<basename>.asm`.

```bash
./tac_vm_translator test_cases/final1
```

On Windows PowerShell (MinGW/WSL recommended), either use a Make environment, or build manually:

```powershell
# Using MinGW make if available
make

# Or compile directly
g++ -std=c++17 -Wall -Iinclude src/*.cpp main.cpp -o tac_vm_translator

# Run
./tac_vm_translator test_cases/final1
```

## Notes

- This is a faithful but simplified C++ port. It supports:
  - Labels ("L:") pass-through
  - Declarations (`- TYPE name`) bookkeeping
  - Assignments and binary arithmetic for INT/BOOL/CHAR
  - GOTO and return
  - Minimal placeholder for IF handling (emits a TODO comment)
- The output is written to `outputs/` folder next to this project.
- Extend `CodeGenerator.cpp` to cover more TAC forms as needed.
