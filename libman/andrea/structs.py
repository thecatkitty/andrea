from ctypes import Structure, c_uint16, c_uint32
from ctypes.wintypes import LONG, WORD


class IMAGE_DOS_HEADER(Structure):
    SIGNATURE = 0x5A4D
    _fields_ = [
        ("e_magic", WORD),
        ("e_cblp", WORD),
        ("e_cp", WORD),
        ("e_crlc", WORD),
        ("e_cparhdr", WORD),
        ("e_minalloc", WORD),
        ("e_maxalloc", WORD),
        ("e_ss", WORD),
        ("e_sp", WORD),
        ("e_csum", WORD),
        ("e_ip", WORD),
        ("e_cs", WORD),
        ("e_lfarlc", WORD),
        ("e_ovno", WORD),
        ("e_res", WORD * 4),
        ("e_oemid", WORD),
        ("e_oeminfo", WORD),
        ("e_res2", WORD * 10),
        ("e_lfanew", LONG),
    ]


class andrea_header(Structure):
    SIGNATURE = 0x61657226
    _pack_ = 2
    _fields_ = [
        ("signature", c_uint32),
        ("size", c_uint16),
        ("num_exports", c_uint16),
        ("num_imports", c_uint16),
        ("size_expstrs", c_uint16),
        ("size_impstrs", c_uint16),
    ]


class andrea_import(Structure):
    _pack_ = 2
    _fields_ = [
        ("_reserved", c_uint16),
        ("name", c_uint16),
    ]
