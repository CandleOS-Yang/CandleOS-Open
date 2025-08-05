#ifndef __ASM_H__
#define __ASM_H__

#include "stdint.h"

/* 获取eip值 */
extern uint32_t get_eip();
/* 跳转高地址内核代码 */
extern void jmp_to_high_kernel();

#endif