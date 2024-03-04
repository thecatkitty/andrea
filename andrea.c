#include "andrea.h"

void
andrea_fptoa(char *buffer, const void far *fp)
{
    uint32_t intptr = (uint32_t)fp;

    for (int i = 7; 0 <= i; i--)
    {
        buffer[i] = '@' + (intptr & 0xF);
        intptr >>= 4;
    }

    buffer[8] = 0;
}

void far *
andrea_atofp(const char far *buffer)
{
    uint32_t intptr = 0;

    for (int i = 0; i < 8; i++)
    {
        intptr <<= 4;
        intptr |= buffer[i] - '@';
    }

    return (void far *)intptr;
}
