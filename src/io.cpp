#ifdef DEBUG
#include <stdio.h>
#include <stdarg.h>
#include "io.hpp"
#endif

void e_printf(const char *format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fflush(stderr);
    va_end(args);
#endif
}

#ifdef DEBUG
static int verbosity = 3;
#else
static int verbosity = 1;
#endif

void increase_verbosity() {
    verbosity ++;
}

int get_verbosity() {
    return verbosity;
}

#ifdef DEBUG
void v_printf(const int v, const char *format, ...) {
    if (verbosity < v) return;
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    fflush(stdout);
    va_end(args);
}
#endif

