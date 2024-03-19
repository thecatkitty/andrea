#include <andrea.h>

static char ANDREA_MODDATA _goodbye[] = "Goodbye World!\r\n$";

#define dos_putfs ANDREA_EXTERN(void, dos_putfs, (const char far *))

void far
goodbye(void)
{
    dos_putfs(_goodbye);
}

ANDREA_EXPORT(goodbye);
