#ifndef __INTERUPT_H__
#define __INTERUPT_H__

#include "stdint.h"

// ÷–∂œ÷°
typedef struct {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t gs;
} int_frame_t;

void enable_int_bit(uint32_t vector);
void disable_int_bit(uint32_t vector);
void send_eoi(uint32_t vector);
void out_exception(uint32_t vector, int_frame_t *esp);
void exception_handler(uint32_t vector, int_frame_t *esp);

#endif