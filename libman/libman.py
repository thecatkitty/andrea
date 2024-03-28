from argparse import ArgumentParser
from io import IOBase
from os.path import basename
from sys import stdout

from andrea import AssemblyGenerator, Module


TEXT_FORMATS = ["text", "asm"]
BINARY_FORMATS = []
FORMATS = TEXT_FORMATS + BINARY_FORMATS


parser = ArgumentParser()
parser.add_argument("module", help="name of the module file")
parser.add_argument("-o", dest="output",
                    help="name of the output file", default="--")
parser.add_argument("-f", dest="format",
                    help="format of the output data", choices=FORMATS)
args = parser.parse_args()

format = args.format if args.format is not None else "asm" if basename(
    args.output).endswith((".asm", ".s", ".S")) else "text"
output = stdout if args.output == "--" else open(
    args.output, "w" if format in TEXT_FORMATS else "wb")
module = Module.from_file(args.module)


def to_text(module: Module, output: IOBase):
    print(f"Container image format {module.format}", file=output)
    print(
        f"Andrea header at {module.header_offset:04X}, size {module.header.size}", file=output)
    print(file=output)

    print("Exports:" if len(module.exports) else "No exports", file=output)
    for i, exp in enumerate(module.exports):
        print(f"  {i}: {(module.cs_base + exp[1]):04X} {exp[0]}", file=output)
    print(file=output)

    print("Imports:" if len(module.imports) else "No imports", file=output)
    for i, imp in enumerate(module.imports):
        print(f"  {i}: {imp}", file=output)
    print(file=output)


def to_asm(module: Module, output: IOBase):
    generator = AssemblyGenerator(module)
    print(generator.emit_prolog(), file=output)

    for chunk in generator.emit_text():
        print(chunk, file=output)

    print(".section .andrea.imports", file=output)
    for chunk in generator.emit_imports():
        print(chunk, file=output)

    print(".section .andrea.impstrs", file=output)
    for chunk in generator.emit_impstrs():
        print(chunk, file=output)


if format == "text":
    to_text(module, output)

elif format == "asm":
    to_asm(module, output)
