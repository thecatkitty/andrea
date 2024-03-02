#include <stdio.h>
#include <stdlib.h>

#include "andrea.h"

unsigned far
callback(unsigned value)
{
    return value * value;
}

int
main(int argc, char *argv[])
{
    char cmd[32] = "module ";
    andrea_fptoa(cmd + 7, callback);

    printf("Calling '%s'\n", cmd);
    int status = system(cmd);
    printf("Returned status: %d\n", status);

    return 0;
}
