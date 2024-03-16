#ifndef _ANDREA_H_
#define _ANDREA_H_

#include <i86.h>

#include <andrea/dosdef.h>

typedef uint16_t andrea_module;

#pragma pack(push, 1)

// Header in the module's TEXT segment, just before the exports table
typedef struct
{
    uint32_t signature;
    uint16_t num_exports;
} andrea_header;

#pragma pack(pop)

#define ANDREA_EXPORT(name)                                                    \
    __attribute__((section(".andrea.exports")))                                \
    const uint16_t __exptbl_##name = FP_OFF(name);                             \
    __attribute__((section(".andrea.strings"))) const char __expstr_##name[] = \
        #name;

#define ANDREA_ORDINAL(ordinal) ((const char far *)(ordinal))

#define ANDREA_SIGNATURE 0x61657226UL

#define ANDREA_MAX_MODULES 5

typedef enum
{
    ANDREA_SUCCESS = 0x00,
    ANDREA_ERROR_MISSING_PARAMETER = 0x80,
    ANDREA_ERROR_INVALID_PARAMETER = 0x81,
    ANDREA_ERROR_TOO_MANY_MODULES = 0x82,
    ANDREA_ERROR_NO_EXPORTS = 0x83,
    ANDREA_ERROR_PROTOCOL_MISMATCH = 0x84,
} andrea_status;

extern andrea_module
andrea_load(const char *name);

extern void
andrea_free(andrea_module module);

extern void far *
andrea_get_procedure(andrea_module module, const char far *name);

extern size_t
andrea_get_procedure_name(void far *procedure, char *buffer, size_t size);

#ifdef ANDREA_LOGS_ENABLE

extern void
andrea_log(const char *location, const char *format, ...);

#define LOG(...) andrea_log(__func__, __VA_ARGS__)

#else // ANDREA_LOGS_ENABLE

#define LOG(...)

#endif // ANDREA_LOGS_ENABLE

#endif // _ANDREA_H_
