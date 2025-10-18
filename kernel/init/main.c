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
#include "string.h"


#define CR0_PE 0x00000001  // 保护模式启用
#define CR0_MP 0x00000002  // 监视协处理器
#define CR0_EM 0x00000004  // 模拟
#define CR0_TS 0x00000008  // 任务切换
#define CR0_ET 0x00000010  // 扩展类型
#define CR0_NE 0x00000020  // 数值错误
#define CR0_WP 0x00010000  // 写保护
#define CR0_AM 0x00040000  // 对齐掩码
#define CR0_NW 0x20000000  // 不可写
#define CR0_CD 0x40000000  // 缓存禁用
#define CR0_PG 0x80000000  // 分页

#define CR4_VME 0x00000001          // 虚拟模式扩展
#define CR4_PVI 0x00000002          // 保护虚拟模式中断
#define CR4_TSD 0x00000004          // 时间戳禁用
#define CR4_DE  0x00000008          // 调试扩展
#define CR4_PSE 0x00000010          // 页面大小扩展
#define CR4_PAE 0x00000020          // 物理地址扩展
#define CR4_MCE 0x00000040          // 机器检查异常
#define CR4_PGE 0x00000080          // 全局页扩展
#define CR4_PCE 0x00000100          // 性能监控计数器扩展
#define CR4_OSFXSR 0x00000200       // 操作系统支持FXSAVE和FXRSTOR指令
#define CR4_OSXMMEXCPT 0x00000400   // 操作系统支持XMM异常
#define CR4_LA57 0x00001000         // 57位线性地址
#define CR4_VMXE 0x00002000         // 虚拟化扩展使能
#define CR4_SMXE 0x00004000         // 安全模式扩展使能

void safe_sse_copy(void *dest, const void *src, size_t bytes);
void sse_enable(void);

mouse_info_t mouse_info;        // 鼠标信息

void kernel_main() {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t *)VBE_MODE_INFO_BASE;
    vbe_init(mode_info);

    mem_init();
    heap_init();
    gdt_init();
    idt_init();
    pic_init();
    keyboard_init();
    mouse_init(&mouse_info);

    sse_enable();

    /*
    1.创建一个SSE.asm文件，用汇编实现memset128和memcpy128函数
    2.修改滚屏函数，使用memset128和memcpy128函数
    */

    uint32_t a = 0;
    while(1) {
        printk("Kernel is running... %d abc\n", a++);
    }
}

void sse_enable(void) {
    uint32_t cr0 = get_cr0();
    uint32_t cr4 = get_cr4();

    set_cr0(cr0 & ~CR0_EM);
    set_cr0(cr0 | CR0_MP);
    set_cr4(cr4 | CR4_OSFXSR | CR4_OSXMMEXCPT);
}