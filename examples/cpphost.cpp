#include <cstdio>

#include <andrea.hpp>

int
main(int argc, char *argv[])
{
    auto module = andrea_module::open("module2.exe");
    if (!module.has_value())
    {
        std::fputs("Cannot open the module!\n", stderr);
        return 1;
    }

    uint32_t procedure = module->get_procedure(1);
    std::printf("Procedure pointer is %08lX.\n", procedure);

    procedure = module->get_procedure("hello");
    std::printf("Procedure pointer is %08lX.\n", procedure);

    return 0;
}
