#include <dos.h>

#include "andrea.h"

__attribute__((section(".text"))) static const char far _hello[] =
    "Hello World!\r\n$";

static inline void
dos_putfs(const char far *str)
{
    __asm volatile("int $0x21"
                   :
                   : "Rah"((unsigned char)0x09), "Rds"(FP_SEG(str)),
                     "d"(FP_OFF(str))
                   : "cc", "memory");
}

void far
hello(void)
{
    dos_putfs(_hello);
}

ANDREA_EXPORT(hello);
