#include "printk.h"
#include "debug.h"
#include "stdint.h"
#include "idt.h"
#include "gdt.h"
#include "pic.h"
#include "interrupt.h"

static char *err_msg[] = {
    "#DE Divide Error",
    "#DB RESERVED",
    "--  NMI Interrupt",
    "#BP Breakpoint",
    "#OF Overflow",
    "#BR BOUND Range Exceeded",
    "#UD Invalid Opcode (Undefined Opcode)",
    "#NM Device Not Available (No Math Coprocessor)",
    "#DF Double Fault",
    "    Coprocessor Segment Overrun (reserved)",
    "#TS Invalid TSS",
    "#NP Segment Not Present",
    "#SS Stack-Segment Fault",
    "#GP General Protection",
    "#PF Page Fault",
    "--  (Intel reserved. Do not use.)",
    "#MF x87 FPU Floating-Point Error (Math Fault)",
    "#AC Alignment Check",
    "#MC Machine Check",
    "#XF SIMD Floating-Point Exception"
};

/* 开启指定中断许可位 */
void enable_int_bit(uint32_t vector) {
    assert(vector >= 0 && vector < IDT_ENTRY_COUNT);

    if (0x20 <= vector && vector < 0x28) {
        uint8_t mask = inb(PIC_DATA_PRIMARY);
        mask &= ~(1 << (vector - 0x20));
        outb(PIC_DATA_PRIMARY, mask);
    } else if (0x28 <= vector && vector < 0x30) {
        uint8_t mask = inb(PIC_DATA_SECONDARY);
        mask &= ~(1 << (vector - 0x28));
        outb(PIC_DATA_SECONDARY, mask);
    }
}

/* 关闭指定中断许可位 */
void disable_int_bit(uint32_t vector) {
    assert(vector >= 0 && vector < IDT_ENTRY_COUNT);

    if (0x20 <= vector && vector < 0x28) {
        uint8_t mask = inb(PIC_DATA_PRIMARY);
        mask |= (1 << (vector - 0x20));
        outb(PIC_DATA_PRIMARY, mask);
    } else if (0x28 <= vector && vector < 0x30) {
        uint8_t mask = inb(PIC_DATA_SECONDARY);
        mask |= (1 << (vector - 0x28));
        outb(PIC_DATA_SECONDARY, mask);
    }
}

/* 发送中断EOI */
void send_eoi(uint32_t vector) {
    assert(vector >= 0 || vector >= IDT_ENTRY_COUNT);
    
    if (0x20 <= vector && vector < 0x28) {
        outb(PIC_CMD_PRIMARY, PIC_EOI);
    } else if (0x28 <= vector && vector < 0x30) {
        outb(PIC_CMD_PRIMARY, PIC_EOI);
        outb(PIC_CMD_SECONDARY, PIC_EOI);
    }
}

/* 获取EIP值 */
uint32_t get_eip() {
    uint32_t eip;
    __asm__ __volatile__(
        "mov eax,[esp]\n"
        "mov %0,eax"
        : "=r"(eip)
        :
    );
    return eip;
}

/* 获取EFLAGS值 */
uint32_t get_eflags() {
    uint32_t eflags;
    __asm__ __volatile__(
        "pushfd\n"
        "pop %0"
        : "=r"(eflags)
        :
    );
    return eflags;
}

/* 输出异常信息 */
void out_exception(uint32_t vector, int_frame_t *esp) {
    uint16_t cs, ss;
    asm volatile(
        "mov %0,cs\n"
        "mov %1,ss\n"
        : "=r"(cs), "=r"(ss)
    );
    printk_color(0xffff0000, "CS: 0x%08x DS: 0x%08x ES: 0x%08x FS: 0x%08x GS: 0x%08x SS: 0x%08x\n", cs, esp->ds, esp->es, esp->fs, esp->gs, ss);
    printk_color(0xffff0000, "EIP: 0x%08x EFLAGS: 0x%08x\n", get_eip(), get_eflags());
    printk_color(0xffff0000, "EAX: 0x%08x EBX: 0x%08x ECX: 0x%08x EDX: 0x%08x\n", esp->eax, esp->ebx, esp->ecx, esp->edx);
    printk_color(0xffff0000, "EDI: 0x%08x ESI: 0x%08x ESP: 0x%08x EBP: 0x%08x\n", esp->edi, esp->esi, esp->esp, esp->ebp);
}

/* 异常处理函数 */
void exception_handler(uint32_t vector, int_frame_t *esp) {
    printk_color(0xffff0000, "!!! Exception Handler\n==> Vector: 0x%x(%s)\nInterrupt Frame:\n", vector, err_msg[vector]);
    out_exception(vector, esp);
    send_eoi(vector);
    while(1);
}