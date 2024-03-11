#pragma once

#include <cstdint>

extern "C" unsigned
__andrea_callfw(uint32_t fp, ...);

template <typename> class andrea_farproc;

template <typename R, typename... Args> class andrea_farproc<R(Args...)>
{
    uint32_t _fp;

  public:
    andrea_farproc(uint32_t fp) : _fp{fp}
    {
    }

    R
    operator()(Args... args)
    {
        return __andrea_callfw(_fp, args...);
    }

    uint32_t
    get() const
    {
        return _fp;
    }
};
