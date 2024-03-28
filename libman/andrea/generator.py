from typing import Generator

from .module import Module


class AssemblyGenerator(object):
    def __init__(self, module: Module) -> None:
        self._module = module

    def emit_prolog(self) -> str:
        return "\n".join([
            ".code16",
            ".intel_syntax noprefix",
            "",
        ])

    def emit_text(self) -> Generator[str, None, None]:
        for name, _ in self._module.exports:
            yield "\n".join([
                f".global {name}",
                f"{name}: ljmp [cs:__imp_{name}]",
            ])
        yield ""

    def emit_imports(self) -> Generator[str, None, None]:
        for name, _ in self._module.exports:
            yield "\n".join([
                f".global __imp_{name}",
                f"__imp_{name}:",
                f".word 0",
                f".word __impstr_{name}",
            ])
        yield ""

    def emit_impstrs(self) -> Generator[str, None, None]:
        for name, _ in self._module.exports:
            yield f"__impstr_{name}: .asciz \"{name}\""
        yield ""
