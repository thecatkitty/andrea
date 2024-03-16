    .arch i8086,jumps
    .code16
    .intel_syntax noprefix

    .text

    .global _start
    .type   _start, @function
_start:
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


    .section .andrea.exports
    .global  andrea_exptabl
andrea_exptabl:
