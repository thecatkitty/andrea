/* Common style settings for all assembly source */

    .arch i8086,jumps
    .code16
    .intel_syntax noprefix


/* Make .text section the default */

    .text


/* Helper macros for routines */

#define GLOBAL_FUN(name)                                                       \
        .global name;                                                          \
        .type   name, @function;                                               \
    name
