#include <andrea/asm.inc>


#define SAVE_RET(base) base + 0
#define SAVE_BP(base)  base + 2


GLOBAL_FUN(__andrea_callfw):
    pop     SAVE_RET(save)
    sub     sp, 2

    mov     SAVE_BP(save), bp
    mov     bp, sp

    add     sp, 6
    lcall   [bp + 2]

    mov     sp, bp
    mov     bp, SAVE_BP(save)

    push    SAVE_RET(save)
    ret


    .data


save:
    .word   0
    .word   0
