	.arch i8086,jumps
	.code16
	.intel_syntax noprefix

	.text

    .global _start
    .type   _start, @function
_start:
    /* Small memory model, DS=SS */
    push    ss
    pop     ds

    /* Call the initialization routine with a far pointer to PSP */
    push    es
    xor     ax, ax
    push    ax
    call    module_init
    add     sp, 4

    /* Exit to caller, status in AL */
    mov     ah, 0x4C
    int     0x21


    .section .preinit
    .global  andrea_exptabl
andrea_exptabl:
