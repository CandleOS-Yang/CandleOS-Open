#include "gdt.h"
#include "printk.h"
#include "debug.h"
#include "stdint.h"

gdt_ptr_t gdt_ptr;                      // GDT指针
gdt_entry_t gdt[GDT_ENTRY_COUNT];       // GDT

/* 初始化GDT表项 */
void gdt_entry_init(gdt_entry_t *entry, uint32_t base, uint32_t limit) {
    entry->base_low = base & 0xffffff;
    entry->base_high = (base >> 24) & 0xff;
    entry->limit_low = limit & 0xffff;
    entry->limit_high = (limit >> 16) & 0xf;
}

/* 初始化GDT */
void gdt_init() {
    memset(gdt, 0, sizeof(gdt));

    // 内核代码段描述符
    gdt_entry_t *code = &gdt[KERNEL_CODE_IDX];
    gdt_entry_init(code, 0, 0xFFFFF);
    code->type = 0b1010;
    code->S = 1;
    code->G = 1;
    code->DPL = 0;
    code->present = 1;
    code->long_mode = 0;
    code->D_B = 1;

    // 内核数据段描述符
    gdt_entry_t *data = &gdt[KERNEL_DATA_IDX];
    gdt_entry_init(data, 0, 0xFFFFF);
    data->type = 0b0010;
    data->S = 1;
    data->G = 1;
    data->DPL = 0;
    data->present = 1;
    data->long_mode = 0;
    data->D_B = 1;

    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uint32_t)gdt;

    asm volatile ("lgdt gdt_ptr\n");
}