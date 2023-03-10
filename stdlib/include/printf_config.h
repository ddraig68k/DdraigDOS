#ifndef DDRAIG_OS_PRINTF_CONFIG_H
#define DDRAIG_OS_PRINTF_CONFIG_H

#define PRINTF_DISABLE_SUPPORT_EXPONENTIAL
#define PRINTF_DISABLE_SUPPORT_LONG_LONG
#define PRINTF_DISABLE_SUPPORT_PTRDIFF_T

extern void (*putchar_func)(char chr, void* arg);

#endif
