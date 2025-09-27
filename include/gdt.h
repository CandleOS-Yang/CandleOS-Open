#ifndef __GDT_H__
#define __GDT_H__

#include "stdint.h"

#define GDT_ENTRY_COUNT 8192            // GDT表项数量

#define KERNEL_CODE_IDX 1               // 内核代码段描述符索引
#define KERNEL_DATA_IDX 2               // 内核数据段描述符索引

#define KERNEL_CODE_SELECTOR (1 << 3)   // 内核代码段选择子
#define KERNEL_DATA_SELECTOR (2 << 3)   // 内核数据段选择子

// GDT指针
typedef struct _packed {
    uint16_t limit;                     // 段界限
    uint32_t base;                      // 段基址
} gdt_ptr_t;

// GDT项
typedef struct _packed {
    uint16_t limit_low;                 // 段界限低位 (0~15位)
    uint32_t base_low : 24;             // 基地址低位 (0~23位)
    uint8_t type : 4;                   // 段类型
    uint8_t S : 1;                      // 系统段/代码数据段标志
    uint8_t DPL : 2;                    // 描述符特权级
    uint8_t present : 1;                // 存在标志位
    uint8_t limit_high : 4;             // 段界限高位 (16~19位)
    uint8_t available : 1;              // 可用位
    uint8_t long_mode : 1;              // 64位扩展标志
    uint8_t D_B : 1;                    // 32位/16位操作尺寸
    uint8_t G : 1;                      // 段界限粒度
    uint8_t base_high;                  // 基地址高位 (24~31位)
} gdt_entry_t;

void gdt_entry_init(gdt_entry_t *entry, uint32_t base, uint32_t limit);
void gdt_init();

#endif