#include <stdarg.h>
#include <stdio.h>

#include <andrea.h>

void
andrea_log(const char *location, const char *format, ...)
{
    va_list args;

    fputs(location, stderr);
    fputs(": ", stderr);

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fputs("\n", stderr);
}
