#include <alloca.h>
#include <dos.h>
#include <libi86/string.h>

#include "host.h"

module_desc __andrea_mods[ANDREA_MAX_MODULES];

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
    andrea_header far *header = __andrea_find_header(FP_SEG(spawn._proc._csip),
                                                     FP_OFF(spawn._proc._csip));
    if (NULL == header)
    {
        LOG("protocol mismatch!");
        _dos_freemem(child);
        status = -ANDREA_ERROR_PROTOCOL_MISMATCH;
        goto end;
    }

    __andrea_fill_moddesc(desc, child, header);
    LOG("loaded, module: %04X, segment: %04X, exports: %u, imports: %u",
        desc->module, desc->segment, header->num_exports, header->num_imports);

end:
    LOG("exit, %d", status);
    return status;
}

andrea_header far *
__andrea_find_header(unsigned segment, unsigned limit)
{
    LOG("entry, segment: %04X, limit: %04X", segment, limit);

    unsigned offset;
    for (offset = 0; offset < limit; offset += 16)
    {
        if (ANDREA_SIGNATURE == *(uint32_t far *)MK_FP(segment, offset))
        {
            break;
        }
    }

    if (offset >= limit)
    {
        LOG("exit, cannot find the signature!");
        return NULL;
    }

    andrea_header far *header = (andrea_header far *)MK_FP(segment, offset);
    if (sizeof(andrea_header) != header->size)
    {
        LOG("exit, invalid header size %u, expected %u!", header->size,
            sizeof(andrea_header));
        return NULL;
    }

end:
    LOG("exit, %04X:%04X", segment, offset);
    return MK_FP(segment, offset);
}

void
__andrea_fill_moddesc(module_desc       *desc,
                      andrea_module      module,
                      andrea_header far *header)
{
    desc->module = module;
    desc->segment = FP_SEG(header);
    desc->exports = FP_OFF(header) + sizeof(andrea_header);
    desc->imports = desc->exports + header->num_exports * sizeof(uint16_t);
    desc->expstrs = desc->imports + header->num_imports * sizeof(andrea_import);
    desc->impstrs = desc->expstrs + header->size_expstrs;
    desc->num_exports = header->num_exports;
    desc->num_imports = header->num_imports;
}

andrea_module
andrea_load(const char *name)
{
    LOG("entry, name: '%s'", name);

    module_desc *desc = moddesc_from_module(0);
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

    uint16_t far *exports = MODDESC_EXPORTS(desc);
    LOG("export table:");
    for (int i = 0; i < desc->num_exports; i++)
    {
        LOG("%3d: %04X", i, exports[i]);
    }

    andrea_import far *imports = MODDESC_IMPORTS(desc);
    LOG("import table:");
    for (int i = 0; i < desc->num_imports; i++)
    {
        const char far *import_name =
            MK_FP(desc->segment, imports[i].desc.name);
        void far *fptr =
            andrea_get_procedure(__andrea_mods[0].module, import_name);

        LOG("%3d: %04X -> %04X:%04X", i, imports[i].desc.name, FP_SEG(fptr),
            FP_OFF(fptr));
        imports[i].fptr = fptr;
    }

    module = desc->module;

end:
    LOG("exit, %04X", module);
    return module;
}
