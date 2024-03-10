#pragma once

#include <cstdint>
#include <stdexcept>

#include <tl/optional.hpp>

class andrea_module
{
    uint16_t _handle;

  public:
    static tl::optional<andrea_module>
    open(const char *name);

    andrea_module(uint16_t handle);
    ~andrea_module();

    uint32_t
    get_procedure(uint16_t ordinal) const;

    uint32_t
    get_procedure(const char *name) const;
};
