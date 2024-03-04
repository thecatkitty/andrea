#include <dos.h>
#include <stddef.h>

#include "andrea.h"

typedef unsigned far (*lpfncallback)(unsigned);

int __errno;

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

char *
utoa10(unsigned value, char *buffer)
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
print(dos_stream stream, const char *str)
{
    unsigned bytes;
    _dos_write(stream, _CV_FP(str), strlen(str), &bytes);
}

uint8_t
module_init(dos_psp far *psp)
{
    print(STDOUT, "Module entry.\r\n");

    if (8 >= psp->cmdline_len)
    {
        print(STDERR, "Module exit, missing parameter.\r\n");
        return ANDREA_ERROR_MISSING_PARAMETER;
    }

    for (int i = 1; i < 9; i++)
    {
        if (('@' > psp->cmdline[i]) || ('O' < psp->cmdline[i]))
        {
            print(STDERR, "Module exit, invalid parameter.\r\n");
            return ANDREA_ERROR_INVALID_PARAMETER;
        }
    }

    lpfncallback callback = (lpfncallback)andrea_atofp(psp->cmdline + 1);

    unsigned value = callback(42);
    print(STDOUT, "Callback returned ");

    char buffer[32];
    utoa10(value, buffer);
    print(STDOUT, buffer);
    print(STDOUT, ".\r\n");

    print(STDOUT, "Module exit, ok.\r\n");
    return ANDREA_SUCCESS;
}
