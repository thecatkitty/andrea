import os
import subprocess

from argparse import ArgumentParser
from io import IOBase
from tempfile import TemporaryDirectory
from sys import stdout

from andrea import AssemblyGenerator, Module


TEXT_FORMATS = ["text", "asm"]
BINARY_FORMATS = ["lib"]
FORMATS = TEXT_FORMATS + BINARY_FORMATS


parser = ArgumentParser()
parser.add_argument("module", help="name of the module file")
parser.add_argument("-o", dest="output",
                    help="name of the output file", default="--")
parser.add_argument("-f", dest="format",
                    help="format of the output data", choices=FORMATS)
args = parser.parse_args()

base = os.path.basename(args.output)
format = args.format if args.format is not None else "asm" if base.endswith(
    (".asm", ".s", ".S")) else "lib" if base.endswith(".a") else "text"
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


def to_lib(module: Module, output: str):
    generator = AssemblyGenerator(module)
    tmpdir = TemporaryDirectory()
    files = list(f"{tmpdir.name}/{name}.o" for name, _ in module.exports)

    for file, text, imports, impstrs in zip(files, generator.emit_text(), generator.emit_imports(), generator.emit_impstrs()):
        gas = subprocess.Popen(
            ["ia16-elf-as", "-o", file], stdin=subprocess.PIPE)
        gas.stdin.writelines(line.encode() for line in [
            generator.emit_prolog(),
            text,
            "\n.section .andrea.imports\n",
            imports,
            "\n.section .andrea.impstrs\n",
            impstrs,
            "\n"
        ])
        gas.stdin.close()
        if 0 != gas.wait():
            raise subprocess.CalledProcessError(gas.returncode, gas.args)

    subprocess.check_call(["ia16-elf-ar", "-rcs", output, *files])
    tmpdir.cleanup()


if format == "text":
    to_text(module, output)

elif format == "asm":
    to_asm(module, output)

elif format == "lib":
    if output == stdout:
        raise ValueError("Cannot write a library to STDOUT")

    output.close()
    os.remove(args.output)
    to_lib(module, args.output)
