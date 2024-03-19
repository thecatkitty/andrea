#include <andrea.h>

__attribute__((section(".text"))) static const char far _hello[] =
    "Hello World!\r\n$";

ANDREA_IMPORT(dos_putfs);
#define dos_putfs ANDREA_EXTERN(void, dos_putfs, (const char far *))

void far
hello(void)
{
    dos_putfs(_hello);
}

ANDREA_EXPORT(hello);
