#if 0
#include <stdio.h>
#include <stdarg.h>
#include "io.hpp"
#endif

void e_printf(const char *format, ...) {
// TODO: Consider implementing e_printf
#if 0
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fflush(stderr);
    va_end(args);
#endif
}

static int verbosity = 1;
void increase_verbosity() {
    verbosity ++;
}

int get_verbosity() {
    return verbosity;
}

#if 0
void v_printf(const int v, const char *format, ...) {
    if (verbosity < v) return;
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    fflush(stdout);
    va_end(args);
}
#endif

