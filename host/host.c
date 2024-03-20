#include <alloca.h>
#include <dos.h>
#include <libi86/string.h>
#include <stdlib.h>

#include "host.h"

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

    fptr = MK_FP(desc->segment, MODDESC_EXPORTS(desc)[ordinal]);

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
        _find_name(MODDESC_EXPSTRS(desc), name, length, desc->num_exports);
    if (desc->num_exports != ordinal)
    {
        fptr = _from_ordinal(desc, ordinal);
    }

    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}

void
andrea_free(andrea_module module)
{
    LOG("entry, module: %04X", module);

    module_desc *desc = moddesc_from_module(module);
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

    module_desc *desc = moddesc_from_module(module);
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
    module_desc *desc = moddesc_from_segment(FP_SEG(procedure));
    if (NULL == desc)
    {
        LOG("module not found!");
        goto end;
    }

    uint16_t far *exports = MODDESC_EXPORTS(desc);

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

    const char far *names = MODDESC_EXPSTRS(desc);
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
