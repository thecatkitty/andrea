#include <stddef.h>

#include <andrea.h>

int __errno;

extern void far
andrea_exptabl(void);

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

    andrea_registration_callback callback =
        (andrea_registration_callback)_deserialize_pointer(psp->cmdline + 1);

    unsigned status = callback((uint16_t far *)andrea_exptabl);

    LOG("exit, status:");
    switch (status)
    {
    case ANDREA_SUCCESS:
        LOG("  success");
        break;

    case ANDREA_ERROR_TOO_MANY_MODULES:
        LOG("  too many modules!");
        break;

    case ANDREA_ERROR_NO_EXPORTS:
        LOG("  no exports!");
        break;

    default:
        LOG("  unknown error!");
        break;
    }

    return status;
}
