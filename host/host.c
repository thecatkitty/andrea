#include <alloca.h>
#include <dos.h>
#include <libi86/string.h>
#include <stdlib.h>

#include <andrea.h>

typedef uint16_t far (*exit_callback)(void);

typedef struct
{
    andrea_module module;
    uint16_t      segment;
    uint16_t      exports;
    uint16_t      imports;
    uint16_t      expstrs;
    uint16_t      impstrs;
    uint16_t      num_exports;
    uint16_t      num_imports;
} module_desc;

extern andrea_hexport __andrea_hexports[];

static module_desc _modules[ANDREA_MAX_MODULES];

#define FIND_DESC(property, value)                                             \
    ({                                                                         \
        module_desc *__ptr = _modules;                                         \
        while ((__ptr < (_modules + ANDREA_MAX_MODULES)) &&                    \
               (__ptr->property != (value)))                                   \
            __ptr++;                                                           \
        (__ptr == (_modules + ANDREA_MAX_MODULES)) ? NULL : __ptr;             \
    })

#define DESC_EXPORTS(desc)                                                     \
    ((uint16_t far *)MK_FP((desc)->segment, (desc)->exports))

#define DESC_IMPORTS(desc)                                                     \
    ((andrea_import far *)MK_FP((desc)->segment, (desc)->imports))

#define DESC_EXPSTRS(desc)                                                     \
    ((const char far *)MK_FP((desc)->segment, (desc)->expstrs))

#define DESC_IMPSTRS(desc)                                                     \
    ((const char far *)MK_FP((desc)->segment, (desc)->impstrs))

static module_desc *
_desc_from_module(andrea_module module)
{
    return FIND_DESC(module, module);
}

static module_desc *
_desc_from_segment(uint16_t segment)
{
    return FIND_DESC(segment, segment);
}

static unsigned
_get_psp(void)
{
    unsigned bx;
    __asm volatile("int $0x21" : "=b"(bx) : "a"(0x5100) : "cc", "memory");
    return bx;
}

static void
_set_psp(unsigned psp)
{
    __asm volatile("int $0x21" : : "a"(0x5000), "b"(psp) : "cc", "memory");
}

static int
_load_module(const char *name, module_desc *desc)
{
    LOG("entry, name: '%s', desc: %04X", name, desc);

    union _dosspawn_t spawn;
    memset(&spawn, 0, sizeof(spawn));

    // Save the parent PSP
    unsigned parent = _get_psp();

    // Load but don't execute
    int status;
    if (0 != (status = _dos_spawn(1, name, &spawn)))
    {
        LOG("cannot spawn!");
        status = -status;
        goto end;
    }

    // Restore the parent PSP
    unsigned child = _get_psp();
    _set_psp(parent);

    // Find the header
    unsigned segment = FP_SEG(spawn._proc._csip), offset;
    for (offset = 0; offset < FP_OFF(spawn._proc._csip); offset += 16)
    {
        if (ANDREA_SIGNATURE == *(uint32_t far *)MK_FP(segment, offset))
        {
            break;
        }
    }

    if (offset >= FP_OFF(spawn._proc._csip))
    {
        LOG("cannot find the signature!");
        _dos_freemem(child);
        status = -ANDREA_ERROR_NO_EXPORTS;
        goto end;
    }

    andrea_header far *header = (andrea_header far *)MK_FP(segment, offset);
    if (sizeof(andrea_header) != header->size)
    {
        LOG("invalid header size %u, expected %u!", header->size,
            sizeof(andrea_header));
        _dos_freemem(child);
        status = -ANDREA_ERROR_PROTOCOL_MISMATCH;
        goto end;
    }

    desc->module = child;
    desc->segment = segment;
    desc->exports = offset + sizeof(andrea_header);
    desc->imports = desc->exports + header->num_exports * sizeof(uint16_t);
    desc->expstrs = desc->imports + header->num_imports * sizeof(andrea_import);
    desc->impstrs = desc->expstrs + header->size_expstrs;
    desc->num_exports = header->num_exports;
    desc->num_imports = header->num_imports;
    LOG("loaded, module: %04X, segment: %04X, exports: %u, imports: %u",
        desc->module, desc->segment, header->num_exports, header->num_imports);

end:
    LOG("exit, %d", status);
    return status;
}

static void far *
_from_ordinal(module_desc *desc, unsigned ordinal)
{
    LOG("entry, desc: %04X, ordinal: %u", desc, ordinal);
    void far *fptr = 0;

    if (desc->num_exports <= ordinal)
    {
        LOG("ordinal too high!");
        goto end;
    }

    fptr = MK_FP(desc->segment, DESC_EXPORTS(desc)[ordinal]);

end:
    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}

static unsigned
_find_name(const char far *names,
           const char far *name,
           size_t          length,
           unsigned        limit)
{
#ifdef ANDREA_LOGS_ENABLE
    char *lname = (char *)alloca(length);
    _fmemcpy(lname, name, length);
    LOG("entry, names: %04X:%04X, name: %s, length: %zu, limit: %u",
        FP_SEG(names), FP_OFF(names), lname, length, limit);
#endif

    for (unsigned i = 0; i < limit; i++)
    {
        if (0 == _fmemcmp(names, name, length))
        {
            LOG("exit, %u", i);
            return i;
        }

        while (*names++)
            ;
    }

    LOG("exit, not found!");
    return limit;
}

static void far *
_from_name(module_desc *desc, const char far *name)
{
    size_t length = _fstrlen(name) + 1;
#ifdef ANDREA_LOGS_ENABLE
    char *lname = (char *)alloca(length);
    _fstrcpy(lname, name);
    LOG("entry, desc: %04X, name: %s", desc, lname);
#endif
    void far *fptr = 0;

    unsigned ordinal =
        _find_name(DESC_EXPSTRS(desc), name, length, desc->num_exports);
    if (desc->num_exports != ordinal)
    {
        fptr = _from_ordinal(desc, ordinal);
    }

    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}

static void far *
_get_hexport(const char far *name)
{
    size_t length = _fstrlen(name) + 1;
#ifdef ANDREA_LOGS_ENABLE
    char *lname = (char *)alloca(length);
    _fstrcpy(lname, name);
    LOG("entry, name: %s", lname);
#endif
    void far *fptr = 0;

    andrea_hexport far *hexport = __andrea_hexports;
    while (NULL != hexport->name)
    {
        if (0 == _fmemcmp(hexport->name, name, length))
        {
            fptr = hexport->fptr;
            break;
        }

        hexport++;
    }

    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}

andrea_module
andrea_load(const char *name)
{
    LOG("entry, name: '%s'", name);

    module_desc *desc = _desc_from_module(0);
    if (NULL == desc)
    {
        return ANDREA_ERROR_TOO_MANY_MODULES;
    }

    andrea_module module = 0;
    if (ANDREA_SUCCESS != _load_module(name, desc))
    {
        goto end;
    }

    if (0 == desc->module)
    {
        return ANDREA_ERROR_NO_EXPORTS;
    }

    uint16_t far *exports = DESC_EXPORTS(desc);
    LOG("export table:");
    for (int i = 0; i < desc->num_exports; i++)
    {
        LOG("%3d: %04X", i, exports[i]);
    }

    andrea_import far *imports = DESC_IMPORTS(desc);
    LOG("import table:");
    for (int i = 0; i < desc->num_imports; i++)
    {
        const char far *import_name =
            MK_FP(desc->segment, imports[i].desc.name);
        void far *fptr = _get_hexport(import_name);

        LOG("%3d: %04X -> %04X:%04X", i, imports[i].desc.name, FP_SEG(fptr),
            FP_OFF(fptr));
        imports[i].fptr = fptr;
    }

    module = desc->module;

end:
    LOG("exit, %04X", module);
    return module;
}

void
andrea_free(andrea_module module)
{
    LOG("entry, module: %04X", module);

    module_desc *desc = _desc_from_module(module);
    if (NULL == desc)
    {
        LOG("exit, double free?");
        return;
    }

    unsigned status = _dos_freemem(desc->module);
    LOG("termination status: %04X", status);

    desc->module = 0;
    LOG("exit");
}

void far *
andrea_get_procedure(andrea_module module, const char far *name)
{
    LOG("entry, module: %04X, name: %04X:%04X", module, FP_SEG(name),
        FP_OFF(name));
    void far *fptr = 0;

    module_desc *desc = _desc_from_module(module);
    if (NULL == desc)
    {
        LOG("not found!");
        goto end;
    }

    fptr = FP_SEG(name) ? _from_name(desc, name)
                        : _from_ordinal(desc, FP_OFF(name));

end:
    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}

size_t
andrea_get_procedure_name(void far *procedure, char *buffer, size_t size)
{
    LOG("entry, procedure: %04X:%04X, buffer: %04X, size: %zu",
        FP_SEG(procedure), FP_OFF(procedure), buffer, size);
    uint16_t module = FP_SEG(procedure), offset = FP_OFF(procedure);

    size_t       length = 0;
    module_desc *desc = _desc_from_segment(FP_SEG(procedure));
    if (NULL == desc)
    {
        LOG("module not found!");
        goto end;
    }

    uint16_t far *exports = DESC_EXPORTS(desc);

    uint16_t ordinal = 0;
    while (offset != exports[ordinal])
    {
        ordinal++;
        if (ordinal >= desc->num_exports)
        {
            LOG("procedure not found!");
            goto end;
        }
    }

    const char far *names = DESC_EXPSTRS(desc);
    for (unsigned i = 0; i < ordinal; i++)
    {
        while (*names++)
            ;
    }

    const char far *end = names;
    while (*end)
    {
        end++;
    }

    length = end - names;
    if (length >= size)
    {
        buffer[0] = 0;
        goto end;
    }

    length++;
    _fmemcpy(buffer, names, length);

end:
    LOG("exit, %zu", length);
    return length;
}
