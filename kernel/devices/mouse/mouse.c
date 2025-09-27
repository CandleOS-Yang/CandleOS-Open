#include "mouse.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"
#include "io.h"
#include "interrupt.h"
#include "idt.h"

/* 鼠标ISR处理函数 */
void mouse_isr_handler(uint32_t vector) {
    uint8_t mouse_data = inb(0x60);
    printk("Mouse Data: 0x%x\n", mouse_data);

    send_eoi(vector);
}