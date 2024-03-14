#include <stddef.h>

#include <andrea.h>

int __errno;

extern andrea_header far __stext;

static void far *
_deserialize_pointer(const char far *buffer)
{
    uint32_t intptr = 0;

    for (int i = 0; i < 8; i++)
    {
        intptr <<= 4;
        intptr |= buffer[i] - '@';
    }

    return (void far *)intptr;
}

uint8_t
module_init(dos_psp far *psp)
{
    LOG("entry");

    if (8 >= psp->cmdline_len)
    {
        LOG("exit, missing parameter");
        return ANDREA_ERROR_MISSING_PARAMETER;
    }

    for (int i = 1; i < 9; i++)
    {
        if (('@' > psp->cmdline[i]) || ('O' < psp->cmdline[i]))
        {
            LOG("exit, invalid parameter");
            return ANDREA_ERROR_INVALID_PARAMETER;
        }
    }

    module_desc far *desc =
        (module_desc far *)_deserialize_pointer(psp->cmdline + 1);
    desc->module = FP_SEG(psp);
    desc->segment = FP_SEG(&__stext);
    desc->exports = FP_OFF(&__stext) + sizeof(andrea_header);
    desc->strings = desc->exports + (__stext.num_exports * sizeof(uint16_t));
    desc->max_ordinal = __stext.num_exports - 1;

    LOG("exit, ok");
    return ANDREA_SUCCESS;
}
