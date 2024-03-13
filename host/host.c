#include <alloca.h>
#include <dos.h>
#include <libi86/string.h>
#include <stdlib.h>

#include <andrea.h>

typedef uint16_t far (*exit_callback)(void);

struct
{
    module_desc modules[ANDREA_MAX_MODULES];
} far _ctx;

static size_t
_find_desc(andrea_module module)
{
    size_t slot;

    for (slot = 0; slot < ANDREA_MAX_MODULES; slot++)
    {
        if (module == _ctx.modules[slot].module)
        {
            break;
        }
    }

    return slot;
}

static void
_serialize_pointer(char *buffer, const void far *fp)
{
    uint32_t intptr = (uint32_t)fp;

    for (int i = 7; 0 <= i; i--)
    {
        buffer[i] = '@' + (intptr & 0xF);
        intptr >>= 4;
    }

    buffer[8] = 0;
}

static int
_load_module(const char *name, const char *cmdline)
{
    LOG("entry, name: '%s', cmdline: '%s'", name, cmdline);

    size_t length = strlen(cmdline);
    length = (length > 125) ? 125 : length;

    char argb[128];
    argb[0] = length + 2;
    argb[1] = ' ';
    strncpy(argb + 2, cmdline, length);
    argb[length + 2] = '\r';

    union _dosspawn_t spawn;
    memset(&spawn, 0, sizeof(spawn));
    spawn._proc_run._argv = _CV_FP(argb);

    int status;
    if (0 != (status = _dos_spawn(0, name, &spawn)))
    {
        LOG("cannot spawn!");
        status = -status;
        goto end;
    }

    _dos_wait(&status);
    if (0x0300 != (status & 0xFF00))
    {
        LOG("module did not stay resident!");
        goto end;
    }

    status &= 0xFF;

end:
    LOG("exit, %d", status);
    return status;
}

static void far *
_from_ordinal(size_t slot, unsigned ordinal)
{
    LOG("entry, slot: %u, ordinal: %u", slot, ordinal);
    void far *fptr = 0;

    module_desc far *desc = _ctx.modules + slot;
    if (desc->max_ordinal < ordinal)
    {
        LOG("ordinal too high!");
        goto end;
    }

    uint16_t far *exports = MK_FP(desc->module, desc->exports);
    fptr = MK_FP(desc->module, exports[ordinal]);

end:
    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}

static void far *
_from_name(size_t slot, const char far *name)
{
    size_t length = _fstrlen(name) + 1;
    {
        char *lname = (char *)alloca(length);
        _fstrcpy(lname, name);
        LOG("entry, slot: %u, name: %s", slot, lname);
    }
    void far *fptr = 0;

    module_desc far *desc = _ctx.modules + slot;
    const char far  *names = MK_FP(desc->module, desc->strings);
    for (unsigned i = 0; i <= desc->max_ordinal; i++)
    {
        if (0 == _fmemcmp(names, name, length))
        {
            fptr = _from_ordinal(slot, i);
            break;
        }

        while (*names++)
            ;
    }

end:
    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}

andrea_module
andrea_load(const char *name)
{
    LOG("entry, name: '%s'", name);

    size_t slot = _find_desc(0);
    if (ANDREA_MAX_MODULES == slot)
    {
        return ANDREA_ERROR_TOO_MANY_MODULES;
    }

    module_desc far *desc = _ctx.modules + slot;

    char ptrstr[9];
    _serialize_pointer(ptrstr, desc);

    andrea_module module = 0;
    if (ANDREA_SUCCESS != _load_module(name, ptrstr))
    {
        goto end;
    }

    LOG("descriptor populated, module: %04X, exports: %04X, strings: %04X, "
        "max_ordinal: %u",
        desc->module, desc->exports, desc->strings, desc->max_ordinal);

    uint16_t far *exports = MK_FP(desc->module, desc->exports);
    LOG("export table:");
    for (int i = 0; i <= desc->max_ordinal; i++)
    {
        LOG("%3d: %04X", i, exports[i]);
    }

    module = _ctx.modules[slot].module;

end:
    LOG("exit, %04X", module);
    return module;
}

void
andrea_free(andrea_module module)
{
    LOG("entry, module: %04X", module);

    size_t slot = _find_desc(module);
    if (ANDREA_MAX_MODULES == slot)
    {
        LOG("exit, double free?");
        return;
    }

    module_desc far *desc = _ctx.modules + slot;
    uint16_t far    *exports = MK_FP(module, desc->exports);

    uint16_t status = ((exit_callback)MK_FP(module, exports[0]))();
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

    size_t slot = _find_desc(module);
    if (ANDREA_MAX_MODULES == slot)
    {
        LOG("not found!");
        goto end;
    }

    fptr = FP_SEG(name) ? _from_name(slot, name)
                        : _from_ordinal(slot, FP_OFF(name));

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

    size_t length = 0;
    size_t slot = _find_desc(FP_SEG(procedure));
    if (ANDREA_MAX_MODULES == slot)
    {
        LOG("module not found!");
        goto end;
    }

    module_desc far *desc = _ctx.modules + slot;
    uint16_t far    *exports = MK_FP(module, desc->exports);

    uint16_t ordinal = 0;
    while (offset != exports[ordinal])
    {
        ordinal++;
        if (ordinal > desc->max_ordinal)
        {
            LOG("procedure not found!");
            goto end;
        }
    }

    const char far *names = MK_FP(desc->module, desc->strings);
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
