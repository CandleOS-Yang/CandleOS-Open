#include "vbe.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"

void kernel_main() {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t*)VBE_MODE_INFO_BASE;
    
    vbe_init(mode_info);

    uint32_t *p = 0xfd000000;
    printk_color(0xffff0000, "Hello, World! p=%p\n", p);
    // panic("This is a panic test p=%p\n", p);
    assert(1 == 0);
}