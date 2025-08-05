#include "mem.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"
#include "vbe.h"
#include "string.h"

MemoryInfo_t mem_info;                          // 内存信息
MemoryPool_t phys_page_pool;                    // 物理页内存池

page_dir_entry_t *kernel_page_dir;              // 内核页目录表
page_table_entry_t *kernel_page_table;          // 内核页表
page_table_entry_t *fb_page_table;              // 帧缓冲区页表

/* 设置cr3寄存器 */
void set_cr3(uint32_t pde_paddr) {
    assert((pde_paddr & 0xfff) == 0);
    asm volatile("mov cr3,eax\n" ::"a"(pde_paddr));
}

/* 开启分页 */
static inline enable_paging() {
    asm volatile (
        "mov eax, cr0\n"
        "or eax, 0x80000000\n"
        "mov cr0, eax\n"
    );
}

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
    
    ("Memory Info\n==> Total Size: %d  Total Pages: %d  Available Base: %p  Available Size: %d  Available Pages: %d\n",
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

/* 初始化内存池 */
void mem_pool_init(MemoryPool_t *mem_pool, void *map_base, uint32_t pool_total_pages, uint32_t map_size) {
    mem_pool->map_base = map_base;
    mem_pool->pool_total_pages = pool_total_pages;
    mem_pool->pool_available_pages = pool_total_pages;
    mem_pool->map_size = map_size;

    uint32_t map_start_idx = (uint32_t)PHYS_MAP_BASE >> 12;
    bitmap_init(&mem_pool->map, (uint8_t *)map_base, map_size, map_start_idx);
    
    uint32_t map_used_pages = CEIL(map_size, PAGE_SIZE);
    for (size_t i = 0; i < map_used_pages; i++) {
        bitmap_set(&mem_pool->map, map_start_idx + i, true);
    }

    mem_info.available_size -= map_size;
    mem_info.available_pages -= map_used_pages;
    mem_pool->pool_available_pages -= map_used_pages;

    printk("Memory Pool\n==> Base: %p  Pages: %d  Map Size: %d\n", map_base, pool_total_pages, map_size);
}

/* 分配物理页 */
void *alloc_phys_pages(uint32_t count) {
    assert(phys_page_pool.pool_available_pages >= count);

    uint32_t idx = bitmap_scan(&phys_page_pool.map, count);
    assert(idx != -1);

    for (size_t i = 0; i < count; i++) {
        bitmap_set(&phys_page_pool.map, idx + i, true);
    }

    mem_info.available_pages -= count;
    mem_info.available_size -= count * PAGE_SIZE;
    phys_page_pool.pool_available_pages -= count;
    
    void *paddr = (void *)(idx << 12);
    printk("Allocated %d Physical Pages\n==> Base: %p\n", count, paddr);
    return paddr;
}

/* 释放物理页 */
void free_phys_pages(void *addr, uint32_t count) {
    assert(addr != NULL);
    assert(count != 0 && count <= phys_page_pool.pool_total_pages);

    uint32_t idx = (uint32_t)addr >> 12;

    for (size_t i = 0; i < count; i++) {
        bitmap_set(&phys_page_pool.map, idx + i, false);
    }

    mem_info.available_pages += count;
    mem_info.available_size += count * PAGE_SIZE;
    phys_page_pool.pool_available_pages += count;

    printk("Freed %d Physical Pages\n==> Base: %p\n", count, addr);
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
    printk("Memory Mapping...\n");

    VbeModeInfo_t *mode_info = (VbeModeInfo_t*)VBE_MODE_INFO_BASE;

    uint32_t fb_size = mode_info->x_resolution * mode_info->y_resolution * 4;
    uint32_t fb_used_pages = CEIL(fb_size, PAGE_SIZE);
    uint32_t fb_pde_idx = (uint32_t)mode_info->framebuffer >> 22;
    
    kernel_page_dir = (page_dir_entry_t *)alloc_phys_pages(1);
    kernel_page_table = (page_table_entry_t *)alloc_phys_pages(KERNEL_PT_COUNTS);
    fb_page_table = (page_table_entry_t *)alloc_phys_pages(CEIL(fb_used_pages, 1024));

    printk("Kernel Page Directory\n==> Base: %p\n", kernel_page_dir);
    printk("Kernel Page Table\n==> Base: %p\n", kernel_page_table);
    printk("Framebuffer Page Table\n==> Base: %p\n", fb_page_table);

    // 映射0x00001000 ~ 0x00FFFFFF物理内存 -> 0x00001000 ~ 0x00FFFFFF虚拟内存
    uint32_t kernel_paddr_idx = 0;
    for (int pde_idx = 0; pde_idx < KERNEL_PDE_COUNTS; pde_idx++) {
        page_dir_entry_t *kernel_pde = (page_dir_entry_t *)&kernel_page_dir[pde_idx];
        page_table_entry_t *kernel_pt = kernel_page_table + pde_idx * 1024;

        memset((void *)kernel_pt, 0, PAGE_SIZE);
        entry_init(kernel_pde, (uint32_t)kernel_pt >> 12);

        for (int pte_idx = 0; pte_idx < 1024; pte_idx++, kernel_paddr_idx++) {
            if (kernel_paddr_idx == 0) {   // 保留0x00000000 ~ 0x00000FFF不映射
                continue;
            }
            
            // printk("Kernel Mapping\n==> 0x%p-0x%p\n", (uint32_t)kernel_paddr_idx << 12, VADDR(pde_idx, pte_idx, 0));
            entry_init(&kernel_pt[pte_idx], kernel_paddr_idx);
        }
    }

    // 映射0x00001000 ~ 0x00FFFFFF物理内存 -> 0xC0001000 ~ 0xC0FFFFFF虚拟内存
    kernel_paddr_idx = 0;
    for (int pde_idx = 0x300; pde_idx < 0x300 + KERNEL_PDE_COUNTS; pde_idx++) {
        page_dir_entry_t *kernel_pde = (page_dir_entry_t *)&kernel_page_dir[pde_idx];
        page_table_entry_t *kernel_pt = kernel_page_table + pde_idx * 1024;

        memset((void *)kernel_pt, 0, PAGE_SIZE);
        entry_init(kernel_pde, (uint32_t)kernel_pt >> 12);

        for (int pte_idx = 0; pte_idx < 1024; pte_idx++, kernel_paddr_idx++) {
            if (kernel_paddr_idx == 0) {   // 保留0x00000000 ~ 0x00000FFF不映射
                continue;
            }
            
            // printk("Kernel Mapping\n==> 0x%p-0x%p\n", (uint32_t)kernel_paddr_idx << 12, VADDR(pde_idx, pte_idx, 0));
            entry_init(&kernel_pt[pte_idx], kernel_paddr_idx);
        }
    }

    uint32_t fb_paddr_idx = (uint32_t)mode_info->framebuffer >> 12;
    page_dir_entry_t *fb_pde = (page_dir_entry_t *)&kernel_page_dir[fb_pde_idx];
    page_table_entry_t *fb_pt = fb_page_table;

    memset((void *)fb_pt, 0, PAGE_SIZE);
    entry_init(fb_pde, (uint32_t)fb_pt >> 12);

    for (int pte_idx = 0; pte_idx < 1024; pte_idx++, fb_paddr_idx++) {
        entry_init(&fb_pt[pte_idx], fb_paddr_idx);
    }

    page_dir_entry_t *last_pde = (page_dir_entry_t *)&kernel_page_dir[1023];
    entry_init(last_pde, (uint32_t)kernel_page_dir >> 12);

    set_cr3((uint32_t)kernel_page_dir);
    enable_paging();

    printk("Paging Mode Is Now Enabled.\n");
}

/* 内存管理初始化 */
void mem_init() {
    get_mem_info((void *)MEM_INFO_BLOCK_BASE);
    mem_pool_init(&phys_page_pool, (void *)PHYS_MAP_BASE, mem_info.available_pages, CEIL(mem_info.available_pages, 8));
    mem_map();

    printk("Memory Management Initialized\n");
}