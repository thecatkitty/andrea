#ifndef _ANDREA_H_
#define _ANDREA_H_

#include <assert.h>
#include <i86.h>
#include <stdint.h>

#ifndef far
#ifdef EDITING
#define far
#else // EDITING
#define far __far
#endif // EDITING
#endif // far

typedef unsigned far (*andrea_registration_callback)(uint16_t far *);

#define ANDREA_EXPORT(name)                                                    \
    __attribute__((section(".preinit"))) const uint16_t __exptbl_##name =      \
        FP_OFF(name);

typedef enum
{
    ANDREA_SUCCESS = 0x00,
    ANDREA_ERROR_MISSING_PARAMETER = 0x80,
    ANDREA_ERROR_INVALID_PARAMETER = 0x81,
} andrea_status;

typedef enum
{
    STDOUT = 1,
    STDERR = 2,
} dos_stream;

#pragma pack(push, 1)
typedef struct
{
    char      int20h[2];
    uint16_t  seg_tail;
    uint8_t   _reserved1;
    char      dispatch[5];
    void far *intv_terminate;
    void far *intv_break;
    void far *intv_error;
    char      _reserved2[22];
    uint16_t  seg_environment;
    char      _reserved3[34];
    char      int21h_retf[3];
    char      _reserved4[9];
    char      fcb1[16];
    char      fcb2[20];
    uint8_t   cmdline_len;
    char      cmdline[127];
} dos_psp;
#pragma pack(pop)

static_assert(256 == sizeof(dos_psp),
              "DOS PSP size doesn't match specification");

#endif
