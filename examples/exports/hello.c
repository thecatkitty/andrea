#include "hexports.h"

static char ANDREA_MODDATA _hello[] = "Hello World!\r\n$";

void far
hello(void)
{
    dos_putfs(_hello);
}

ANDREA_EXPORT(hello);
