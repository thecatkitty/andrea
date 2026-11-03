#include <alloca.h>
#include <libi86/string.h>

#include "host.h"

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
_find_procedure(module_desc *desc, const char far *name)
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
        fptr = MK_FP(desc->segment, MODDESC_EXPORTS(desc)[ordinal]);
    }

    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
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

    fptr = _find_procedure(desc, name);

end:
    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}
