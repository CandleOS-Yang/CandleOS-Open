#include "vbe.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"
#include "mem.h"
#include "heap.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "mouse.h"

void kernel_main() {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t *)VBE_MODE_INFO_BASE;
    vbe_init(mode_info);

    mem_init();
    heap_init();
    gdt_init();
    idt_init();
    pic_init();
    keyboard_init();


    while(1);
}