#include <stdio.h>
#include <stdlib.h>

#include "andrea.h"

typedef unsigned far (*lpfncallback)(unsigned);

int
main(int argc, char *argv[])
{
    if (2 != argc)
    {
        return ANDREA_ERROR_MISSING_PARAMETER;
    }

    for (int i = 0; i < 8; i++)
    {
        if (('@' > argv[1][i]) || ('O' < argv[1][i]))
        {
            return ANDREA_ERROR_INVALID_PARAMETER;
        }
    }

    lpfncallback callback = (lpfncallback)andrea_atofp(argv[1]);

    unsigned value = callback(42);
    fputs("Callback returned ", stdout);

    char buffer[32];
    __utoa(value, buffer, 10);
    fputs(buffer, stdout);
    fputs(".\n", stdout);

    return 0;
}
