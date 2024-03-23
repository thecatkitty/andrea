#include <i86.h>
#include <libi86/stdlib.h>

#include "host.h"

extern void
_start(void);

void
andrea_init(void)
{
    LOG("entry");

    andrea_header far *header =
        __andrea_find_header(__libi86_get_cs(), (unsigned)_start);
    __andrea_fill_moddesc(__andrea_mods + 0, __libi86_psp, header);

    LOG("exit");
}
