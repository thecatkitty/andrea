#include <andrea.h>

typedef struct
{
    andrea_module module;
    uint16_t      segment;
    uint16_t      exports;
    uint16_t      imports;
    uint16_t      expstrs;
    uint16_t      impstrs;
    uint16_t      num_exports;
    uint16_t      num_imports;
} module_desc;

#define ANDREA_MAX_MODULES 5

extern module_desc __andrea_mods[ANDREA_MAX_MODULES];

#define MODDESC_FIND(property, value)                                          \
    ({                                                                         \
        module_desc *__ptr = __andrea_mods;                                    \
        while ((__ptr < (__andrea_mods + ANDREA_MAX_MODULES)) &&               \
               (__ptr->property != (value)))                                   \
            __ptr++;                                                           \
        (__ptr == (__andrea_mods + ANDREA_MAX_MODULES)) ? NULL : __ptr;        \
    })

#define MODDESC_EXPORTS(desc)                                                  \
    ((uint16_t far *)MK_FP((desc)->segment, (desc)->exports))

#define MODDESC_IMPORTS(desc)                                                  \
    ((andrea_import far *)MK_FP((desc)->segment, (desc)->imports))

#define MODDESC_EXPSTRS(desc)                                                  \
    ((const char far *)MK_FP((desc)->segment, (desc)->expstrs))

#define MODDESC_IMPSTRS(desc)                                                  \
    ((const char far *)MK_FP((desc)->segment, (desc)->impstrs))

inline static module_desc *
moddesc_from_module(andrea_module module)
{
    return MODDESC_FIND(module, module);
}

inline static module_desc *
moddesc_from_segment(uint16_t segment)
{
    return MODDESC_FIND(segment, segment);
}
