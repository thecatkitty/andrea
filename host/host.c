#include <alloca.h>
#include <dos.h>
#include <libi86/string.h>
#include <stdlib.h>

#include <andrea.h>

typedef uint16_t far (*exit_callback)(void);

static module_desc _modules[ANDREA_MAX_MODULES];

#define FIND_DESC(property, value)                                             \
    ({                                                                         \
        module_desc *__ptr = _modules;                                         \
        while ((__ptr < (_modules + ANDREA_MAX_MODULES)) &&                    \
               (__ptr->property != (value)))                                   \
            __ptr++;                                                           \
        (__ptr == (_modules + ANDREA_MAX_MODULES)) ? NULL : __ptr;             \
    })

#define DESC_EXPORTS(desc)                                                     \
    ((uint16_t far *)MK_FP((desc)->segment, (desc)->exports))

#define DESC_NAMES(desc)                                                       \
    ((const char far *)MK_FP((desc)->segment, (desc)->strings))

static module_desc *
_desc_from_module(andrea_module module)
{
    return FIND_DESC(module, module);
}

static module_desc *
_desc_from_segment(uint16_t segment)
{
    return FIND_DESC(segment, segment);
}

static void
_serialize_pointer(char *buffer, const void far *fp)
{
    uint32_t intptr = (uint32_t)fp;

    for (int i = 7; 0 <= i; i--)
    {
        buffer[i] = '@' + (intptr & 0xF);
        intptr >>= 4;
    }

    buffer[8] = 0;
}

static int
_load_module(const char *name, const char *cmdline)
{
    LOG("entry, name: '%s', cmdline: '%s'", name, cmdline);

    size_t length = strlen(cmdline);
    length = (length > 125) ? 125 : length;

    char argb[128];
    argb[0] = length + 2;
    argb[1] = ' ';
    strncpy(argb + 2, cmdline, length);
    argb[length + 2] = '\r';

    union _dosspawn_t spawn;
    memset(&spawn, 0, sizeof(spawn));
    spawn._proc_run._argv = _CV_FP(argb);

    int status;
    if (0 != (status = _dos_spawn(0, name, &spawn)))
    {
        LOG("cannot spawn!");
        status = -status;
        goto end;
    }

    _dos_wait(&status);
    if (0x0300 != (status & 0xFF00))
    {
        LOG("module did not stay resident!");
        goto end;
    }

    status &= 0xFF;

end:
    LOG("exit, %d", status);
    return status;
}

static void far *
_from_ordinal(module_desc *desc, unsigned ordinal)
{
    LOG("entry, desc: %04X, ordinal: %u", desc, ordinal);
    void far *fptr = 0;

    if (desc->max_ordinal < ordinal)
    {
        LOG("ordinal too high!");
        goto end;
    }

    fptr = MK_FP(desc->segment, DESC_EXPORTS(desc)[ordinal]);

end:
    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}

static void far *
_from_name(module_desc *desc, const char far *name)
{
    size_t length = _fstrlen(name) + 1;
#ifdef ANDREA_LOGS_ENABLE
    char *lname = (char *)alloca(length);
    _fstrcpy(lname, name);
    LOG("entry, desc: %04X, name: %s", desc, lname);
#endif
    void far *fptr = 0;

    const char far *names = DESC_NAMES(desc);
    for (unsigned i = 0; i <= desc->max_ordinal; i++)
    {
        if (0 == _fmemcmp(names, name, length))
        {
            fptr = _from_ordinal(desc, i);
            break;
        }

        while (*names++)
            ;
    }

end:
    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}

andrea_module
andrea_load(const char *name)
{
    LOG("entry, name: '%s'", name);

    module_desc *desc = _desc_from_module(0);
    if (NULL == desc)
    {
        return ANDREA_ERROR_TOO_MANY_MODULES;
    }

    char ptrstr[9];
    _serialize_pointer(ptrstr, desc);

    andrea_module module = 0;
    if (ANDREA_SUCCESS != _load_module(name, ptrstr))
    {
        goto end;
    }

    LOG("descriptor populated, module: %04X, segment: %04X, exports: %04X, "
        "strings: %04X, max_ordinal: %u",
        desc->module, desc->segment, desc->exports, desc->strings,
        desc->max_ordinal);

    uint16_t far *exports = DESC_EXPORTS(desc);
    LOG("export table:");
    for (int i = 0; i <= desc->max_ordinal; i++)
    {
        LOG("%3d: %04X", i, exports[i]);
    }

    module = desc->module;

end:
    LOG("exit, %04X", module);
    return module;
}

void
andrea_free(andrea_module module)
{
    LOG("entry, module: %04X", module);

    module_desc *desc = _desc_from_module(module);
    if (NULL == desc)
    {
        LOG("exit, double free?");
        return;
    }

    unsigned status = _dos_freemem(desc->module);
    LOG("termination status: %04X", status);

    desc->module = 0;
    LOG("exit");
}

void far *
andrea_get_procedure(andrea_module module, const char far *name)
{
    LOG("entry, module: %04X, name: %04X:%04X", module, FP_SEG(name),
        FP_OFF(name));
    void far *fptr = 0;

    module_desc *desc = _desc_from_module(module);
    if (NULL == desc)
    {
        LOG("not found!");
        goto end;
    }

    fptr = FP_SEG(name) ? _from_name(desc, name)
                        : _from_ordinal(desc, FP_OFF(name));

end:
    LOG("exit, %04X:%04X", FP_SEG(fptr), FP_OFF(fptr));
    return fptr;
}

size_t
andrea_get_procedure_name(void far *procedure, char *buffer, size_t size)
{
    LOG("entry, procedure: %04X:%04X, buffer: %04X, size: %zu",
        FP_SEG(procedure), FP_OFF(procedure), buffer, size);
    uint16_t module = FP_SEG(procedure), offset = FP_OFF(procedure);

    size_t       length = 0;
    module_desc *desc = _desc_from_segment(FP_SEG(procedure));
    if (NULL == desc)
    {
        LOG("module not found!");
        goto end;
    }

    uint16_t far *exports = DESC_EXPORTS(desc);

    uint16_t ordinal = 0;
    while (offset != exports[ordinal])
    {
        ordinal++;
        if (ordinal > desc->max_ordinal)
        {
            LOG("procedure not found!");
            goto end;
        }
    }

    const char far *names = DESC_NAMES(desc);
    for (unsigned i = 0; i < ordinal; i++)
    {
        while (*names++)
            ;
    }

    const char far *end = names;
    while (*end)
    {
        end++;
    }

    length = end - names;
    if (length >= size)
    {
        buffer[0] = 0;
        goto end;
    }

    length++;
    _fmemcpy(buffer, names, length);

end:
    LOG("exit, %zu", length);
    return length;
}
