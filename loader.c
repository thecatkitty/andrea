#include <process.h>
#include <stdio.h>

#include "andrea.h"

typedef uint16_t far (*exit_callback)(void);

static andrea_module far _last_module;
static uint16_t far      _last_exports;

static char _callback_ptr[9] = "";

static unsigned far
_callback(uint16_t far *exports)
{
    _last_module = FP_SEG(exports);
    _last_exports = FP_OFF(exports);

    return 0;
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

    return _last_module;
}

void
andrea_free(andrea_module module)
{
    uint16_t far *exports = MK_FP(module, _last_exports);

    uint16_t status = ((exit_callback)MK_FP(module, exports[0]))();
    printf("Module termination status %04X.\n", status);
}

void far *
andrea_get_procedure(andrea_module module, uint16_t ordinal)
{
    uint16_t far *exports = MK_FP(module, _last_exports);
    for (int i = 0; i <= ordinal; i++)
    {
        if (0 == exports[i])
        {
            return 0;
        }
    }

    return MK_FP(module, exports[ordinal]);
}
