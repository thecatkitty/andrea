#include <process.h>
#include <stdio.h>

#include <andrea.h>

typedef uint16_t far (*exit_callback)(void);

typedef struct
{
    andrea_module module;
    uint16_t      exports;
    uint16_t      max_ordinal;
} module_desc;

struct
{
    module_desc modules[ANDREA_MAX_MODULES];
    size_t      last_slot;
} far _ctx;

static char _callback_ptr[9] = "";

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

static unsigned far
_callback(uint16_t far *exports)
{
    size_t slot = _find_desc(0);
    if (ANDREA_MAX_MODULES == slot)
    {
        return ANDREA_ERROR_TOO_MANY_MODULES;
    }

    if (0 == exports[0])
    {
        return ANDREA_ERROR_NO_EXPORTS;
    }

    module_desc far *desc = _ctx.modules + slot;
    desc->module = FP_SEG(exports);
    desc->exports = FP_OFF(exports);

    uint16_t ordinal = 1;
    while (0 != exports[ordinal])
    {
        ordinal++;
    }
    desc->max_ordinal = ordinal - 1;

    _ctx.last_slot = slot;
    return ANDREA_SUCCESS;
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

andrea_module
andrea_load(const char *name)
{
    if (0 == _callback_ptr[0])
    {
        _serialize_pointer(_callback_ptr, _callback);
    }

    printf("Spawning module '%s' with '%s'\n", name, _callback_ptr);
    int status = _spawnl(P_WAIT, name, name, _callback_ptr, NULL) & 0xFF;
    printf("Returned status: %d\n", status);

    if (ANDREA_SUCCESS != status)
    {
        return 0;
    }

    size_t           slot = _ctx.last_slot;
    module_desc far *desc = _ctx.modules + slot;
    uint16_t far    *exports = MK_FP(desc->module, desc->exports);
    printf("Export table of the module '%s' at %04X:\n", name, desc->module);
    for (int i = 0; i <= desc->max_ordinal; i++)
    {
        printf("%3d: %04X\n", i, exports[i]);
    }
    return _ctx.modules[slot].module;
}

void
andrea_free(andrea_module module)
{
    size_t slot = _find_desc(module);
    if (ANDREA_MAX_MODULES == slot)
    {
        fputs("Module double free?\n", stderr);
        return;
    }

    module_desc far *desc = _ctx.modules + slot;
    uint16_t far    *exports = MK_FP(module, desc->exports);

    uint16_t status = ((exit_callback)MK_FP(module, exports[0]))();
    printf("Module termination status %04X.\n", status);

    desc->module = 0;
}

void far *
andrea_get_procedure(andrea_module module, uint16_t ordinal)
{
    size_t slot = _find_desc(module);
    if (ANDREA_MAX_MODULES == slot)
    {
        fputs("Not found!\n", stderr);
        return 0;
    }

    module_desc far *desc = _ctx.modules + slot;
    if (desc->max_ordinal < ordinal)
    {
        fputs("Ordinal!\n", stderr);
        return 0;
    }

    printf(
        "Module %04X: slot: %u, module: %04X, exports: %04X, max_ordinal: %u\n",
        module, slot, desc->module, desc->exports, desc->max_ordinal);

    uint16_t far *exports = MK_FP(module, desc->exports);
    return MK_FP(module, exports[ordinal]);
}
