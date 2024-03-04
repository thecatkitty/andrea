#include <stdio.h>
#include <stdlib.h>

#include "andrea.h"

typedef int far (*lpfnsquare)(int);

static int far _export_count;
static int far _value;

static unsigned far
_registrator(uint16_t far *exports)
{
    lpfnsquare square = (lpfnsquare)MK_FP(FP_SEG(exports), exports[0]);
    _value = square(42);

    for (_export_count = 0; exports[_export_count]; _export_count++)
        ;

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

int
main(int argc, char *argv[])
{
    char cmd[32] = "module ";
    _serialize_pointer(cmd + 7, _registrator);

    printf("Calling '%s'\n", cmd);
    int status = system(cmd);
    printf("Returned status: %d\n", status);
    printf("%d exports\n", _export_count);
    printf("42 squared is %d\n", _value);

    return 0;
}
