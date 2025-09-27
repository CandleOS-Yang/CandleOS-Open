#include "pic.h"
#include "io.h"
#include "debug.h"
#include "stdint.h"

/* PIC初始化 */
void pic_init() {
    // 初始化主片
    outb(PIC_CMD_PRIMARY, PIC_ICW1);                // 边沿触发_级联模式_需要ICW4
    outb(PIC_DATA_PRIMARY, 0x20);                   // 中断起始向量0x20
    outb(PIC_DATA_PRIMARY, PIC_ICW3_PRIMARY);       // IR2连接从片
    outb(PIC_DATA_PRIMARY, PIC_ICW4);               // 8086模式_手动 EOI
    
    // 初始化从片
    outb(PIC_CMD_SECONDARY, PIC_ICW1);              // 边沿触发_级联模式_需要ICW4
    outb(PIC_DATA_SECONDARY, 0x28);                 // 中断起始向量0x28
    outb(PIC_DATA_SECONDARY, PIC_ICW3_SECONDARY);   // IR2连接主片
    outb(PIC_DATA_SECONDARY, PIC_ICW4);             // 8086模式_手动 EOI

    // 屏蔽部分中断
    outb(PIC_DATA_PRIMARY, PIC_OCW1_PRIMARY);
    outb(PIC_DATA_SECONDARY, PIC_OCW1_SECONDARY);
    
    // 允许中断产生
    asm volatile ("sti\n");
}