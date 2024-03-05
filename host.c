#include <stdio.h>

#include "andrea.h"

typedef int far (*lpfnsquare)(int);

int
main(int argc, char *argv[])
{
    andrea_module module = andrea_load("module");
    if (0 == module)
    {
        fprintf(stderr, "Cannot load module!\n");
        return 1;
    }

    lpfnsquare square = (lpfnsquare)andrea_get_procedure(module, 1);
    if (0 == square)
    {
        fprintf(stderr, "Cannot get procedure!\n");
        return 1;
    }

    printf("42 squared is %d\n", square(42));
    andrea_free(module);

    printf("Host end.\n");

    return 0;
}
