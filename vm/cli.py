import sys, json, os
from .assembler import assemble
from .disassembler import disassemble
from .vmachine import VM

def main():
    if len(sys.argv) < 4:
        print("Usage: cli.py [asm|dis|run] input output")
        sys.exit(1)

    mode, inp, out = sys.argv[1:4]

    if mode == "asm":
        with open(inp) as f: lines = f.readlines()
        bc, labels = assemble(lines)
        with open(out, "wb") as f: f.write(bc)
        with open(out + ".labels.json", "w") as f: json.dump(labels, f, indent=2)

    elif mode == "dis":
        with open(inp, "rb") as f: bc = f.read()

        # try loading labels JSON
        labels_file = inp + ".labels.json"
        labels = {}
        if os.path.exists(labels_file):
            with open(labels_file) as f:
                labels = json.load(f)

        asm = disassemble(bc, labels)
        with open(out, "w") as f:
            f.write("\n".join(asm))

    elif mode == "run":
        with open(inp, "rb") as f: bc = f.read()
        vm = VM(bc)
        stack = vm.run()
        with open(out, "w") as f: f.write("Final stack: " + repr(stack))

    else:
        print("Invalid mode")
        sys.exit(2)

if __name__ == "__main__":
    main()
