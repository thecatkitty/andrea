#include <dos.h>
#include <stdarg.h>
#include <stdio.h>

#include <andrea.h>

size_t
strlen(const char *str)
{
    size_t count = 0;

    while (*str++)
    {
        count++;
    }

    return count;
}

static void
_print(dos_stream stream, const char *str)
{
    unsigned bytes;
    _dos_write(stream, _CV_FP(str), strlen(str), &bytes);
}

void
andrea_log(const char *location, const char *format, ...)
{
    _print(STDERR, location);
    _print(STDERR, ": ");

    _print(STDERR, format);

    _print(STDERR, "\r\n");
}
