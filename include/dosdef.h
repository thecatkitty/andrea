#ifndef _DOSDEF_H_
#define _DOSDEF_H_

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#ifndef far
#ifdef EDITING
#define far
#else // EDITING
#define far __far
#endif // EDITING
#endif // far

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

#endif // _DOSDEF_H_
