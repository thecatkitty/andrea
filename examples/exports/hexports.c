#include <andrea.h>

void far
dos_putfs(const char far *str)
{
    __asm volatile("int $0x21"
                   :
                   : "Rah"((unsigned char)0x09), "Rds"(FP_SEG(str)),
                     "d"(FP_OFF(str))
                   : "cc", "memory");
}

ANDREA_HEXPORTS{ANDREA_HEXPORT(dos_putfs), ANDREA_HEXPORTS_END};
