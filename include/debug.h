#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "stdint.h"

#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        assert_fail(#exp, __FILE__, __LINE__)

void assert_fail(const char *exp, const char *file, int line);
void panic(const char *fmt, ...);

#endif