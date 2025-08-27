#include "vbe.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"
#include "mem.h"
#include "heap.h"

void kernel_main() {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t *)VBE_MODE_INFO_BASE;
    vbe_init(mode_info);

    mem_init();
    heap_init();
    asm ("xchg bx,bx");
    gdt_init();

    while(1);
}