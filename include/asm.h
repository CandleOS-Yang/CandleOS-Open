#ifndef __ASM_H__
#define __ASM_H__

#include "stdint.h"

/* ��ȡeipֵ */
extern uint32_t get_eip();
/* ��ת�ߵ�ַ�ں˴��� */
extern void jmp_to_high_kernel();

#endif