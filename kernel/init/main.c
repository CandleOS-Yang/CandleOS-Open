#include "vbe.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"
#include "mem.h"
#include "asm.h"

void kernel_main() {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t*)VBE_MODE_INFO_BASE;
    
    vbe_init(mode_info);
    mem_init();
    jmp_to_high_kernel();
    printk("Jmp To High Kernel Success! EIP=0x%p\n", (uint32_t)get_eip());

    while(1);
}