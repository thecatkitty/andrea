#include <stdio.h>

#include <andrea.h>

typedef int  far (*lpfnsquare)(int);
typedef void far (*lpfnhello)(void);

static andrea_module
_try_load_module(const char *name)
{
    andrea_module module = andrea_load(name);
    if (0 == module)
    {
        fprintf(stderr, "Cannot load '%s'!\n", name);
    }
    else
    {
        printf("'%s' loaded as %04X!\n", name, module);
    }

    return module;
}

static void far *
_try_get_procedure(andrea_module module, uint16_t ordinal)
{
    void far *procedure = andrea_get_procedure(module, ordinal);
    if (0 == procedure)
    {
        fprintf(stderr, "Cannot get procedure %u of %04X!\n", ordinal, module);
        goto end;
    }

    printf("Got procedure %u: %04X:%04X", ordinal, FP_SEG(procedure),
           FP_OFF(procedure));

    char name[32];
    if (andrea_get_procedure_name(procedure, name, sizeof(name)))
    {
        printf(" (%s)", name);
    }
    puts("");

end:
    return procedure;
}

#define TRY_STEP(variable, expression)                                         \
    if (0 == ((variable) = (expression)))                                      \
    {                                                                          \
        return 1;                                                              \
    }                                                                          \
    getchar();

#define STEP(message, block)                                                   \
    {                                                                          \
        puts("'" message "' start");                                           \
        block;                                                                 \
        puts("'" message "' end");                                             \
        getchar();                                                             \
    }

int
main(int argc, char *argv[])
{
    puts("Host start");

    andrea_module module1;
    TRY_STEP(module1, _try_load_module("module1.exe"));

    andrea_module module2;
    TRY_STEP(module2, _try_load_module("module2.exe"));

    lpfnsquare square;
    TRY_STEP(square, (lpfnsquare)_try_get_procedure(module1, 1));

    STEP("Call square", printf("42 squared is %d\n", square(42)));

    STEP("Free module1", andrea_free(module1));

    lpfnhello hello;
    TRY_STEP(hello, (lpfnhello)_try_get_procedure(module2, 1));

    STEP("Call hello", hello());

    STEP("Free module2", andrea_free(module2));

    puts("Host end");
    return 0;
}
