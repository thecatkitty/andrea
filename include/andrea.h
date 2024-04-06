#ifndef _ANDREA_H_
#define _ANDREA_H_

#include <i86.h>

#include <andrea/dosdef.h>

///
/// Handle to a loaded Andrea module.
///
typedef uint16_t andrea_module;

#pragma pack(push, 1)

///
/// Header in the module's TEXT segment, just before the exports table.
///
typedef struct
{
    uint32_t signature;
    uint16_t size;
    uint16_t num_exports;
    uint16_t num_imports;
    uint16_t size_expstrs;
    uint16_t size_impstrs;
} andrea_header;

///
/// Entry inside of the import table.
///
typedef union {
    struct
    {
        uint16_t _reserved;
        uint16_t name;
    } desc;

    void far *fptr;
} andrea_import;

#pragma pack(pop)

///
/// Marks a procedure as exported by name.
/// Relies on the linker script containing the export table.
///
/// @param name Name of the exported procedure.
///
#define ANDREA_EXPORT(name)                                                    \
    __attribute__((section(".andrea.exports"))) const uint16_t __exp_##name =  \
        FP_OFF(name);                                                          \
    __attribute__((section(".andrea.expstrs"))) const char __expstr_##name[] = \
        #name;

///
/// Imports a procedure by name without using the import library.
/// Relies on the linker script containing the import table.
///
/// @param name Name of the imported procedure.
///
#define ANDREA_IMPORT(name)                                                    \
    __attribute__((weak)) __attribute__((section(".andrea.impstrs")))          \
    const char __impstr_##name[] = #name;                                      \
    __attribute__((weak)) __attribute__((section(".andrea.imports")))          \
    andrea_import far __imp_##name = {{0, (unsigned)__impstr_##name}};

///
/// Declares an imported procedure.
/// Relies on either the import library or the `ANDREA_IMPORT(...)` macro being
/// used.
///
/// @param type Return type of the imported procedure.
/// @param name Name of the imported procedure.
/// @param args Call arguments of the imported procedure, inside parentheses.
///
#define ANDREA_EXTERN(type, name, args)                                        \
    ({                                                                         \
        extern andrea_import far __imp_##name;                                 \
        ((type far(*) args)__imp_##name.fptr);                                 \
    })

///
/// Marks static data as usable in the module code.
///
#define ANDREA_MODDATA __attribute__((section(".andrea.moddata"))) far

///
/// Converts a number into an argument of `andrea_get_procedure(...)`.
///
/// @param ordinal Ordinal number of the requested procedure.
///
#define ANDREA_ORDINAL(ordinal) ((const char far *)(ordinal))

///
/// Signature of any Andrea module - `&rea` in little endian.
///
#define ANDREA_SIGNATURE 0x61657226UL

///
/// Return status of Andrea API calls.
///
typedef enum
{
    ANDREA_SUCCESS = 0x00,
    ANDREA_ERROR_MISSING_PARAMETER = 0x80,
    ANDREA_ERROR_INVALID_PARAMETER = 0x81,
    ANDREA_ERROR_TOO_MANY_MODULES = 0x82,
    ANDREA_ERROR_NO_EXPORTS = 0x83,
    ANDREA_ERROR_PROTOCOL_MISMATCH = 0x84,
} andrea_status;

///
/// Initializes the Andrea host, registering its exports.
///
extern void
andrea_init(void);

///
/// Loads an Andrea module to memory.
///
/// @param name File name of the requested module.
///
/// @return Handle to the loaded module, 0 on error.
///
extern andrea_module
andrea_load(const char *name);

///
/// Releases a previously loaded Andrea module.
///
/// @param module Handle to the released module.
///
extern void
andrea_free(andrea_module module);

///
/// Retrieves a procedure exported by an Andrea module.
///
/// @param module Handle to the loaded module.
/// @param name Name of the requested procedure, or `ANDREA_ORDINAL(...)`.
///
/// @return Far pointer to the procedure, NULL on error.
///
extern void far *
andrea_get_procedure(andrea_module module, const char far *name);

///
/// Retrieves the name of an imported procedure.
///
/// @param procedure Far pointer to the procedure.
/// @param buffer Output buffer for the retrieved name.
/// @param size Total size of the buffer.
///
/// @return Length of the name, 0 if not found, 1 if anonymous.
///         If the length is greater than the size of the buffer, only a null
///         character is written.
///
extern size_t
andrea_get_name(void far *procedure, char *buffer, size_t size);

#ifdef ANDREA_LOGS_ENABLE

///
/// Write a formatted message to the STDERR if logging is enabled.
/// Used by `LOG(...)`.
///
/// @param location Name of the logged subroutine.
/// @param format Formatting string of the message.
/// @param ... Arguments of the formatted message.
///
extern void
andrea_log(const char *location, const char *format, ...);

///
/// Write a formatted message to the STDERR if logging is enabled.
///
/// @param ... Formatting string of the message with zero or more arguments.
///
#define LOG(...) andrea_log(__func__, __VA_ARGS__)

#else // ANDREA_LOGS_ENABLE

#define LOG(...)

#endif // ANDREA_LOGS_ENABLE

#endif // _ANDREA_H_
