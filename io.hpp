#pragma once

void e_printf(const char *format, ...);
inline void v_printf(const int v, const char *format, ...) {}

void increase_verbosity();
int get_verbosity();
