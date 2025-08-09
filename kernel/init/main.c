#include "vbe.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"
#include "mem.h"
#include "asm.h"

void kernel_main() {
    mem_init();
    asm volatile ("xchg bx,bx");
    while(1);
    
    // vbe_init(mode_info);

    // printk("Jmp To High Kernel Success! EIP=0x%p\n", (uint32_t)get_eip());

    while(1);
}