#ifndef _ANDREA_H_
#define _ANDREA_H_

#include <i86.h>

#include <dosdef.h>

typedef unsigned far (*andrea_registration_callback)(uint16_t far *);
typedef uint16_t andrea_module;

#define ANDREA_EXPORT(name)                                                    \
    __attribute__((section(".preinit"))) const uint16_t __exptbl_##name =      \
        FP_OFF(name);

#define ANDREA_MAX_MODULES 5

typedef enum
{
    ANDREA_SUCCESS = 0x00,
    ANDREA_ERROR_MISSING_PARAMETER = 0x80,
    ANDREA_ERROR_INVALID_PARAMETER = 0x81,
    ANDREA_ERROR_TOO_MANY_MODULES = 0x82,
    ANDREA_ERROR_NO_EXPORTS = 0x83,
} andrea_status;

extern andrea_module
andrea_load(const char *name);

extern void
andrea_free(andrea_module module);

extern void far *
andrea_get_procedure(andrea_module module, uint16_t ordinal);

#ifdef ANDREA_LOGS_ENABLE

extern void
andrea_log(const char *location, const char *format, ...);

#define LOG(...) andrea_log(__func__, __VA_ARGS__)

#else // ANDREA_LOGS_ENABLE

#define LOG(...)

#endif // ANDREA_LOGS_ENABLE

#endif // _ANDREA_H_
