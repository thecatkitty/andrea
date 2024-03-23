from ctypes import c_uint16, c_uint32, sizeof
from io import IOBase
from .structs import IMAGE_DOS_HEADER, andrea_header, andrea_import


class Module(object):
    def __init__(self, file: IOBase) -> None:
        mz_bytes = file.read(sizeof(IMAGE_DOS_HEADER))
        mz = IMAGE_DOS_HEADER.from_buffer_copy(mz_bytes)
        if mz.e_magic != IMAGE_DOS_HEADER.SIGNATURE:
            raise ValueError("Not an MZ EXE file")

        self.format = "MZ"
        if mz.e_lfarlc >= sizeof(IMAGE_DOS_HEADER):
            file.seek(mz.e_lfanew)
            signature = file.read(2).decode()
            if signature.isalpha() and signature.isupper():
                self.format = signature

        offset = 0x10 * mz.e_cparhdr
        while offset < mz.e_ip:
            file.seek(offset)
            signature_bytes = file.read(sizeof(c_uint32))
            signature = c_uint32.from_buffer_copy(signature_bytes)
            if signature.value == andrea_header.SIGNATURE:
                break
            offset += 0x10

        if offset >= mz.e_ip:
            raise ValueError("Andrea header not found")

        file.seek(offset)
        andrea_bytes = file.read(sizeof(andrea_header))
        andrea = andrea_header.from_buffer_copy(andrea_bytes)
        if sizeof(andrea_header) != andrea.size:
            raise ValueError("Protocol mismatch")

        exports_type = c_uint16 * andrea.num_exports
        exports_bytes = file.read(sizeof(exports_type))
        exports = list(exports_type.from_buffer_copy(exports_bytes))

        imports_type = andrea_import * andrea.num_imports
        imports_bytes = file.read(sizeof(imports_type))
        imports = list(imports_type.from_buffer_copy(imports_bytes))

        expstrs_bytes = file.read(andrea.size_expstrs)
        expstrs = expstrs_bytes.decode().split("\0")

        impstrs_base = file.tell()
        impstrs_bytes = file.read(andrea.size_impstrs)

        self.header = andrea
        self.header_offset = offset
        self.cs_base = 0x10 * (mz.e_cparhdr + mz.e_cs) & 0xFFFF
        self.exports = list(zip(expstrs, exports))
        self.imports = list(impstrs_bytes[(
            imp.name - impstrs_base):].decode().split("\0")[0] for imp in imports)

    @staticmethod
    def from_file(path: str) -> "Module":
        with open(path, "rb") as file:
            return Module(file)
