#include "stdint.h"
#include "stdio.h"
#include "stdarg.h"
#include "printk.h"
#include "vbe.h"

/* 内核格式化输出 */
void printk(const char *fmt, ...) {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t*)VBE_MODE_INFO_BASE;
    char formt_buffer[1024] = {0};

    va_list ap;
    va_start(ap, fmt);
    int len = vsprintf(formt_buffer, fmt, ap);
    va_end(ap);

    vbe_put_string(mode_info, 0xffffffff, formt_buffer);
}

/* 内核格式化输出-自定义颜色 */
void printk_color(uint32_t color, const char *fmt, ...) {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t*)VBE_MODE_INFO_BASE;
    char formt_buffer[1024] = {0};

    va_list ap;
    va_start(ap, fmt);
    int len = vsprintf(formt_buffer, fmt, ap);
    va_end(ap);

    vbe_put_string(mode_info, color, formt_buffer);
}