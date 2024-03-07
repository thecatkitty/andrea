#include <stdio.h>

#include "andrea.h"

typedef int far (*lpfnsquare)(int);
typedef void far (*lpfnhello)(void);

int
main(int argc, char *argv[])
{
    andrea_module module = andrea_load("module");
    if (0 == module)
    {
        fprintf(stderr, "Cannot load module!\n");
        return 1;
    }
    printf("Module loaded as %04X!\n", module);
    getchar();

    andrea_module module2 = andrea_load("module2");
    if (0 == module2)
    {
        fprintf(stderr, "Cannot load module2!\n");
        return 1;
    }
    printf("Module2 loaded as %04X!\n", module2);
    getchar();

    lpfnsquare square = (lpfnsquare)andrea_get_procedure(module, 1);
    if (0 == square)
    {
        fprintf(stderr, "Cannot get procedure!\n");
        return 1;
    }
    printf("Square got: %04X:%04X\n", FP_SEG(square), FP_OFF(square));
    getchar();

    printf("42 squared is %d\n", square(42));
    puts("Square called!");
    getchar();

    andrea_free(module);
    puts("Module freed!");
    getchar();

    lpfnhello hello = (lpfnhello)andrea_get_procedure(module2, 1);
    if (0 == hello)
    {
        fprintf(stderr, "Cannot get procedure!\n");
        return 1;
    }
    printf("Hello got: %04X:%04X\n", FP_SEG(hello), FP_OFF(hello));
    getchar();

    hello();
    puts("Hello called!");
    getchar();

    andrea_free(module2);
    puts("Module2 freed!");
    getchar();

    printf("Host end.\n");

    return 0;
}
