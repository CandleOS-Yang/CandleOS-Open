#include "mem.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"
#include "vbe.h"
#include "string.h"

/*
在loader开启分页
内核映射0xC0000000，大小1GB
*/

MemoryInfo_t mem_info;                          // 内存信息
MemoryPool_t phys_pool;                  // 物理页内存池
MemoryPool_t kernel_pool;               // 内核虚拟页内存池

/* 获取内存信息 */
void get_mem_info(void *ards) {
    uint32_t ards_counts = *(uint32_t *)ards;
    Ards_t *ards_ptr = (Ards_t *)((uint32_t)ards + sizeof(uint32_t));

    printk("Memory Block:\n");
    for (int i = 0; i < ards_counts; i++, ards_ptr++) {
        printk("==> Base: 0x%p  Size: %d  Type: %d\n",
            (uint32_t)ards_ptr->base,
            (uint32_t)ards_ptr->size,
            (uint32_t)ards_ptr->type);
            
        mem_info.total_size += ards_ptr->size;
        if (ards_ptr->type == ARDS_TYPE_AVAILABLE && ards_ptr->size > mem_info.available_size) {
            mem_info.available_base = (uint32_t)ards_ptr->base;
            mem_info.available_size = (uint32_t)ards_ptr->size;
        }
    }

    mem_info.total_pages = CEIL(mem_info.total_size, PAGE_SIZE);
    mem_info.available_pages = CEIL(mem_info.available_size, PAGE_SIZE);
    
    printk("Memory Info\n==> Total Size: %d  Total Pages: %d  Available Base: %p  Available Size: %d  Available Pages: %d\n",
        CEIL(mem_info.total_size, MB_SIZE),
        mem_info.total_pages,
        mem_info.available_base,
        CEIL(mem_info.available_size, MB_SIZE),
        mem_info.available_pages);
}

/* 获取虚拟地址信息 */
void get_vaddr_info(void *vaddr, vaddr_info_t *vaddr_info) {
    uint32_t ptr = (uint32_t)vaddr;

    vaddr_info->page_dir_idx = (ptr >> 22) & 0x3ff;
    vaddr_info->page_table_idx = (ptr >> 12) & 0x3ff;
    vaddr_info->page_offset = ptr & 0xfff;
}

/* 内存池初始化 */
void mem_pool_init() {
    
}

/* 项初始化 */
static void entry_init(entry_t *entry, uint32_t index)
{
    *(uint32_t *)entry = 0;
    entry->present = 1;
    entry->write = 1;
    entry->user = 1;
    entry->index = index;
}

/* 内存映射 */
void mem_map() {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t*)VBE_MODE_INFO_BASE;
    uint32_t fb_size = mode_info->x_resolution * mode_info->y_resolution * 4;
    uint32_t fb_used_pages = CEIL(fb_size, PAGE_SIZE);
    uint32_t fb_pde_idx = (uint32_t)mode_info->framebuffer >> 22;
    uint32_t fb_pt_counts = CEIL(fb_used_pages, 1024);
    
    uint32_t fb_paddr_idx = (uint32_t)mode_info->framebuffer >> 12;
    page_dir_entry_t *fb_pde = (page_dir_entry_t *)(KERNEL_PAGE_DIR_BASE + fb_pde_idx * 4);
    page_table_entry_t *fb_pt = (page_table_entry_t *)FB_PAGE_TABLE_BASE;

    for (int pde_idx = fb_pde_idx; pde_idx < fb_pde_idx + fb_pt_counts; pde_idx++) {
        page_dir_entry_t *fb_pde = (page_dir_entry_t *)(KERNEL_PAGE_DIR_BASE + fb_pde_idx * 4);
        page_table_entry_t *fb_pt = FB_PAGE_TABLE_BASE + pde_idx * 1024;

        memset((void *)fb_pt, 0, PAGE_SIZE);
        entry_init(fb_pde, (uint32_t)fb_pt >> 12);

        for (int pte_idx = 0; pte_idx < 1024; pte_idx++, fb_paddr_idx++) {
            entry_init(&fb_pt[pte_idx], fb_paddr_idx);
        }
    }

    asm volatile ("xchg bx,bx");
    printk("Paging Mode Is Now Enabled.\n");
}


/* 内存管理初始化 */
void mem_init() {
    mem_map();
    asm volatile ("xchg bx,bx");

    get_mem_info((void *)MEM_INFO_BLOCK_BASE);

    printk("Hello\n");
}