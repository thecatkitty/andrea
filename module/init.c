#include <dos.h>
#include <stddef.h>

#include "andrea.h"

int __errno;

extern void far
andrea_exptabl(void);

size_t
strlen(const char *str)
{
    size_t count = 0;

    while (*str++)
    {
        count++;
    }

    return count;
}

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

static char *
_utoa10(unsigned value, char *buffer)
{
    int digits = 9999 < value  ? 5
                 : 999 < value ? 4
                 : 99 < value  ? 3
                 : 9 < value   ? 2
                               : 1;

    for (int i = 1; i <= digits; i++)
    {
        buffer[digits - i] = '0' + (value % 10);
        value /= 10;
    }

    buffer[digits] = 0;
    return buffer;
}

static void
_print(dos_stream stream, const char *str)
{
    unsigned bytes;
    _dos_write(stream, _CV_FP(str), strlen(str), &bytes);
}

uint8_t
module_init(dos_psp far *psp)
{
    _print(STDOUT, "Module entry.\r\n");

    if (8 >= psp->cmdline_len)
    {
        _print(STDERR, "Module exit, missing parameter.\r\n");
        return ANDREA_ERROR_MISSING_PARAMETER;
    }

    for (int i = 1; i < 9; i++)
    {
        if (('@' > psp->cmdline[i]) || ('O' < psp->cmdline[i]))
        {
            _print(STDERR, "Module exit, invalid parameter.\r\n");
            return ANDREA_ERROR_INVALID_PARAMETER;
        }
    }

    andrea_registration_callback callback =
        (andrea_registration_callback)_deserialize_pointer(psp->cmdline + 1);

    unsigned status = callback((uint16_t far *)andrea_exptabl);
    _print(STDOUT, "Callback returned ");

    char buffer[32];
    _utoa10(status, buffer);
    _print(STDOUT, buffer);
    _print(STDOUT, ".\r\n");

    _print(STDOUT, "Module exit.\r\n");
    return status;
}
