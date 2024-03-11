#include <cstdio>

#include <andrea.hpp>
#include <andrea/farproc.hpp>

int
main(int argc, char *argv[])
{
    auto module1 = andrea_module::open("module1.exe");
    if (!module1.has_value())
    {
        std::fputs("Cannot open the module1!\n", stderr);
        return 1;
    }

    auto module2 = andrea_module::open("module2.exe");
    if (!module2.has_value())
    {
        std::fputs("Cannot open the module2!\n", stderr);
        return 1;
    }

    andrea_farproc<unsigned(unsigned)> square{module1->get_procedure("square")};
    std::printf("Procedure pointer is %08lX.\n", square.get());
    std::printf("Square of 42 is %u\n", square(42));

    andrea_farproc<unsigned()> hello{module2->get_procedure("hello")};
    std::printf("Procedure pointer is %08lX.\n", hello.get());
    hello();

    return 0;
}
