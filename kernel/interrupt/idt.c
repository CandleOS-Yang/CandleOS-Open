#include "printk.h"
#include "debug.h"
#include "stdint.h"
#include "idt.h"
#include "gdt.h"
#include "pic.h"
#include "interrupt.h"

idt_ptr_t idt_ptr;                                      // IDT指针
gate_descriptor_t idt[IDT_ENTRY_COUNT];                 // IDT

extern uint32_t *isr_entry_table[IDT_ENTRY_COUNT];      // ISR入口函数表
uint32_t *isr_handler_table[IDT_ENTRY_COUNT];           // ISR处理函数表

/* 初始化门描述符 */
void gate_descriptor_init(gate_descriptor_t *gate, uint32_t selector, uint32_t offset) {
    gate->selector = selector;
    gate->offset_low = offset & 0xffff;
    gate->offset_high = (offset >> 16) & 0xffff;
}

/* 默认ISR处理函数 */
void default_isr_handler(uint32_t vector) {
    printk("==> Have a default isr handler!\nInterrups Vector: 0x%x\n", vector);
    send_eoi(vector);
}

/* 设置ISR处理函数地址 */
void set_isr_handler(uint32_t vector, uint32_t handler) {
    assert(vector >= 0 && vector < IDT_ENTRY_COUNT);
    isr_handler_table[vector] = handler;
}

/* 初始化IDT */
void idt_init() {
    memset(idt, 0, sizeof(idt));
    
    for (int i = 0; i < IDT_ENTRY_COUNT; i++) {
        gate_descriptor_init(&idt[i], KERNEL_CODE_SELECTOR, (uint32_t)isr_entry_table[i]);

        idt[i].type = GATE_TYPE_32_INT;
        idt[i].S = 0;
        idt[i].DPL = 0;
        idt[i].P = 1;
    }

    for (int i = 0; i < 32; i++) {
        isr_handler_table[i] = exception_handler;
    }

    idt_ptr.base = (uint32_t)&idt;
    idt_ptr.limit = sizeof(idt) - 1;

    asm volatile ("lidt idt_ptr\n");
}