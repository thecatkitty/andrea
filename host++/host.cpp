#include <libi86/internal/farptr.h>

#include <andrea.hpp>

extern "C" uint16_t
andrea_load(const char *name);

extern "C" void
andrea_free(uint16_t module);

extern "C" __libi86_fpcv_t
andrea_get_procedure(uint16_t module, __libi86_fpcv_t name);

tl::optional<andrea_module>
andrea_module::open(const char *name)
{
    auto handle = andrea_load(name);
    if (handle)
    {
        return tl::make_optional<andrea_module>(handle);
    }

    return tl::optional<andrea_module>();
}

andrea_module::andrea_module(uint16_t handle) : _handle{handle}
{
}

andrea_module::~andrea_module()
{
    if (_handle)
    {
        andrea_free(_handle);
    }
}

uint32_t
andrea_module::get_procedure(uint16_t ordinal) const
{
    auto fp = andrea_get_procedure(_handle, __libi86_fpcv_t(ordinal));
    return *(uint32_t *)&fp;
}

uint32_t
andrea_module::get_procedure(const char *name) const
{
    auto fp = andrea_get_procedure(
        _handle, __libi86_fpcv_t(__libi86_CV_FP(const_cast<char *>(name))));
    return *(uint32_t *)&fp;
}
