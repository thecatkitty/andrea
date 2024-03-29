#include "hexports.h"

static char ANDREA_MODDATA _goodbye[] = "Goodbye World!\r\n$";

void far
goodbye(void)
{
    dos_putfs(_goodbye);
}

ANDREA_EXPORT(goodbye);
