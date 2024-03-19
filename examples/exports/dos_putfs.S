    .arch i8086,jumps
    .code16
    .intel_syntax noprefix


    .text


    .global dos_putfs
dos_putfs:
    /* Save the stack frame */
    push    bp
    mov     bp, sp

    /* Load string address to DS:DX */
    lds     dx, [bp + 6]
    mov     ah, 9
    int     0x21

    /* Restore the stack frame and DS */
    pop     bp
    push    ss
    pop     ds
    retf
