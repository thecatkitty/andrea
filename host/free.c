#include <dos.h>

#include "host.h"

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
