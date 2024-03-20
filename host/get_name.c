#include <libi86/string.h>

#include "host.h"

size_t
andrea_get_name(void far *procedure, char *buffer, size_t size)
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
