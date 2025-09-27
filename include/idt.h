#ifndef __IDT_H__
#define __IDT_H__

#include "stdint.h"

#define IDT_ENTRY_COUNT 256            // IDT表项数量

#define GATE_TYPE_16_INT 0x6           // 16位中断门
#define GATE_TYPE_16_TRAP 0x7          // 16位陷阱门
#define GATE_TYPE_32_INT 0xE           // 32位中断门
#define GATE_TYPE_32_TRAP 0xF          // 32位陷阱门

// IDT指针
typedef struct _packed {
    uint16_t limit;                     // 段界限
    uint32_t base;                      // 段基址
} idt_ptr_t;

// IDT表项
typedef struct _packed {
    uint16_t offset_low;                // 偏移地址 (0~15位)
    uint16_t selector;                  // 目标段选择子
    uint8_t reserved;                   // 保留
    uint8_t type : 4;                   // 类型
    uint8_t S : 1;                      // 段类型
    uint8_t DPL : 2;                    // DPL特权级
    uint8_t P : 1;                      // 存在位
    uint16_t offset_high;               // 偏移地址 (16~31位)
} gate_descriptor_t;

void idt_entry_init(gate_descriptor_t *entry, uint32_t base, uint32_t limit);
void default_isr_handler(uint32_t vector);
void set_isr_handler(uint32_t vector, uint32_t handler);
void idt_init();

#endif