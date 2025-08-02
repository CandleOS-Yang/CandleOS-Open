#ifndef __STDIO_H__
#define __STDIO_H__

#include "stdarg.h"
#include "stdint.h"

static int skip_atoi(const char **s);
static char *number(char *str, uint32_t *num, int base, int size, int precision, int flags);
int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);

#endif