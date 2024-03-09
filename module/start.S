    .arch i8086,jumps
    .code16
    .intel_syntax noprefix

    .text

module_psp:
    .word   0

    .global _start
    .type   _start, @function
_start:
    mov     cs:[module_psp], es

    /* Small memory model, DS=SS */
    push    ss
    pop     ds

    /* Call the initialization routine with a far pointer to PSP */
    push    es
    xor     ax, ax
    push    ax
    call    module_init
    add     sp, 4

    cmp     al, 0
    jne     .no_tsr

    /* Exit to caller and stay resident, status in AL */
    mov     ah, 0x31
    mov     dx, (offset __etext_padded) + 0x200
    add     dx, (offset __ebss) + 15
    mov     cl, 4
    shr     dx, cl
    int     0x21

.no_tsr:
    /* Exit to caller and release resources, status in AL */
    mov     ah, 0x4C
    int     0x21


exit_module: 
    push    es

    /* Release module's PSP */
    mov     ax, cs:[module_psp]
    mov     es, ax
    mov     ax, 0x4900
    int     0x21
    jc      .return
    xor     ax, ax

.return:
    pop     es
    retf


    .section .preinit
    .global  andrea_exptabl
andrea_exptabl:
    .word   exit_module
