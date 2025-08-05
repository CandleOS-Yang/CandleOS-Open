#include "stdint.h"
#include "printk.h"
#include "stdarg.h"
#include "stdio.h"
#include "vbe.h"

/* ¶ÏÑÔÊ§°Ü´¦Àí */
void assert_fail(const char *exp, const char *file, int line) {
    printk_color(0xffff0000, "!!! Assertion failed: %s\n==> file: %s, line: %d\n", exp, file, line);
}

/* PANIC */
void panic(const char *fmt, ...) {
    char formt_buffer[1024] = {0};

    va_list ap;
    va_start(ap, fmt);
    int len = vsprintf(formt_buffer, fmt, ap);
    va_end(ap);

    printk_color(0xffff0000, "!!! panic\n==> %s \n", formt_buffer);
    while (1);
}