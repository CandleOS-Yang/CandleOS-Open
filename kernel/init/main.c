#include "vbe.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"

void kernel_main() {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t*)VBE_MODE_INFO_BASE;
    
    vbe_init(mode_info);
    // printk("Hello, world!\n");
    mem_init();

    while(1);
}