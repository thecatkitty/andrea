import argparse

from andrea.module import Module

parser = argparse.ArgumentParser()
parser.add_argument("module", help="name of the module file")

args = parser.parse_args()

module = Module.from_file(args.module)

print(f"Container image format {module.format}")
print(f"Andrea header at {module.header_offset:04X}, size {module.header.size}")
print()

print("Exports:" if len(module.exports) else "No exports")
for i, exp in enumerate(module.exports):
    print(f"  {i}: {(module.cs_base + exp[1]):04X} {exp[0]}")
print()

print("Imports:" if len(module.imports) else "No imports")
for i, imp in enumerate(module.imports):
    print(f"  {i}: {imp}")
print()
