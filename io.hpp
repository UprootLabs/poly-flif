#pragma once

void e_printf(const char *format, ...);

#ifdef DEBUG
void v_printf(const int v, const char *format, ...);
#else
inline void v_printf(const int v, const char *format, ...) {}
#endif

void increase_verbosity();
int get_verbosity();
