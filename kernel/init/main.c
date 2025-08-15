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

    void *p1 = kmalloc(16);
    void *p2 = kmalloc(68);
    void *p3 = kmalloc(2048);

    printk("p1: %p\n", p1);
    printk("p2: %p\n", p2);
    printk("p3: %p\n", p3);
    
    while(1);
}