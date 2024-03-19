#include <andrea/asm.inc>


GLOBAL_FUN(_start):
    /* Tiny memory model, DS=CS */
    push    cs
    pop     ds

    /* Print out the message */
    lea     dx, .message
    mov     ah, 0x09
    int     0x21

    /* Exit to caller and release resources, status=1 */
    mov     ax, 0x4C01
    int     0x21

.message:
    .ascii  "This is an Andrea loadable module.\r\n$"
