#include "mem.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"
#include "vbe.h"
#include "string.h"

MemoryInfo_t mem_info;                      // 内存信息
MemoryPool_t phys_pool;                     // 物理页内存池
MemoryPool_t kernel_pool;                   // 内核虚拟页内存池

// 功能型函数
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
void get_vaddr_info(vaddr_info_t *vaddr_info, void *vaddr) {
    uint32_t ptr = (uint32_t)vaddr;

    vaddr_info->page_dir_idx = (ptr >> 22) & 0x3ff;
    vaddr_info->page_table_idx = (ptr >> 12) & 0x3ff;
    vaddr_info->page_offset = ptr & 0xfff;
}

/* 项初始化 */
void entry_init(entry_t *entry, uint32_t index)
{
    *(uint32_t *)entry = 0;
    entry->present = 1;
    entry->write = 1;
    entry->user = 1;
    entry->index = index;
}

/* 映射一页内存 */
void map_a_page(void *vaddr, void *paddr) {
    vaddr_info_t vaddr_info;
    get_vaddr_info(&vaddr_info, vaddr);

    page_dir_entry_t *pde = (page_dir_entry_t *)(KERNEL_PAGE_DIR_BASE + vaddr_info.page_dir_idx * 4);
    page_table_entry_t *pt = (page_table_entry_t *)(KERNEL_PAGE_TABLE_BASE + vaddr_info.page_dir_idx * PAGE_SIZE);
    if (pde->present == 0) {
        memset(pt, 0, PAGE_SIZE);
        entry_init(pde, ((uint32_t)pt - KERNEL_VIRTUAL_BASE) >> 12);
    }

    page_table_entry_t *pte = (page_table_entry_t *)((uint32_t)pt + vaddr_info.page_table_idx * 4);
    if (pte->present == 0) {
        entry_init(pte, (uint32_t)paddr >> 12);
    }
}



// 具体函数
/* 内存池初始化 */
void mem_pool_init() {
    phys_pool.pool_base = (void *)mem_info.available_base;
    phys_pool.pool_total_pages = mem_info.available_pages;
    phys_pool.pool_available_pages = mem_info.available_pages;
    phys_pool.pool_map_base = PHYS_MAP_BASE;
}

/* 内存管理初始化 */
void mem_init() {
    get_mem_info((void *)MEM_INFO_BLOCK_BASE);

    asm volatile ("xchg bx,bx");
    uint32_t *p = 0x500000;

    asm volatile ("xchg bx,bx");
    map_a_page(p, 0x10000);
    asm volatile ("xchg bx,bx");

    *p = 0xfd000000;
    asm volatile ("xchg bx,bx");
}