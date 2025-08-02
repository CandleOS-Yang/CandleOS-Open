#ifndef _PRINTK_H_
#define _PRINTK_H_

#include "stdint.h"

void printk(const char *fmt, ...);
void printk_color(uint32_t color, const char *fmt, ...);

#endif