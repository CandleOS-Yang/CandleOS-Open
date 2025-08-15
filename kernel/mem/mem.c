#include "mem.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"
#include "vbe.h"
#include "string.h"
#include "heap.h"

MemoryInfo_t mem_info;                      // 内存信息
MemoryPool_t phys_pool;                     // 物理页内存池
MemoryPool_t kernel_pool;                   // 内核虚拟页内存池

uint32_t low1MB_pt_counts;                  // 低端1MB内存页表数量
uint32_t kernel_pt_counts;                  // 内核页表数量
uint32_t fb_pt_counts;                      // 帧缓冲区页表数量

uint32_t fb_page_table_base;                // 帧缓冲区页表虚拟地址

// 功能型函数
/* 获取内存信息 */
void get_mem_info(void *ards) {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t *)VBE_MODE_INFO_BASE;
    uint32_t ards_counts = *(uint32_t *)ards;
    Ards_t *ards_ptr = (Ards_t *)((uint32_t)ards + sizeof(uint32_t));

    memset(&mem_info, 0, sizeof(MemoryInfo_t));

    printk("Memory Block:\n");
    for (int i = 0; i < ards_counts; i++, ards_ptr++) {
        printk("==> Base: 0x%p  Size: %d  Type: %d\n",
            (uint32_t)ards_ptr->base,
            (uint32_t)ards_ptr->size,
            (uint32_t)ards_ptr->type);
            
        if (ards_ptr->type == ARDS_TYPE_AVAILABLE) {
            mem_info.total_size += ards_ptr->size;
            if (ards_ptr->size > mem_info.available_size) {
                mem_info.available_base = (uint32_t)ards_ptr->base;
                mem_info.available_size = (uint32_t)ards_ptr->size;
            }
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

    page_dir_entry_t *pde = (page_dir_entry_t *)(KERNEL_PAGE_DIR_VADDR + vaddr_info.page_dir_idx * 4);
    page_table_entry_t *pt = (page_table_entry_t *)(KERNEL_PAGE_TABLE_VADDR + vaddr_info.page_dir_idx * PAGE_SIZE);
    if (pde->present == 0) {
        memset(pt, 0, PAGE_SIZE);
        entry_init(pde, ((uint32_t)pt - KERNEL_VADDR) >> 12);
    }

    page_table_entry_t *pte = (page_table_entry_t *)((uint32_t)pt + vaddr_info.page_table_idx * 4);
    if (pte->present == 0) {
        entry_init(pte, (uint32_t)paddr >> 12);
    }
}

// 具体函数
/* 分配物理页 */
void *alloc_phys_pages(uint32_t count) {
    assert(count <= phys_pool.pool_available_pages);

    uint32_t idx = bitmap_scan(&phys_pool.pool_map, count);
    for (int i = 0; i < count; i++) {
        bitmap_set(&phys_pool.pool_map, idx + i, true);
    }
    
    printk("Allocated Phys Pages\n==> Base: %p\n", (void *)(idx << 12));
    return (void *)(idx << 12);
}

/* 释放物理页 */
void free_phys_pages(void *paddr, uint32_t count) {
    assert((uint32_t)paddr >= phys_pool.pool_base);
    assert((uint32_t)paddr < (uint32_t)(phys_pool.pool_base + phys_pool.pool_total_pages * PAGE_SIZE - 1));

    uint32_t idx = (uint32_t)paddr >> 12;
    for (int i = 0; i < count; i++) {
        bitmap_set(&phys_pool.pool_map, idx + i, false);
    }

    printk("Freed Phys Pages\n==> Base: %p\n", (void *)paddr);
}

/* 分配内核页 */
void *alloc_kernel_pages(uint32_t count) {
    assert(count <= kernel_pool.pool_available_pages);

    uint32_t idx = bitmap_scan(&kernel_pool.pool_map, count);
    for (int i = 0; i < count; i++) {
        bitmap_set(&kernel_pool.pool_map, idx + i, true);
    }
    
    printk("Allocated Kernel Pages\n==> Base: %p\n", (void *)(idx << 12));
    return (void *)(idx << 12);
}

/* 释放内核页 */
void free_kernel_pages(void *vaddr, uint32_t count) {
    assert((uint32_t)vaddr >= kernel_pool.pool_base);
    assert((uint32_t)vaddr < (uint32_t)(kernel_pool.pool_base + kernel_pool.pool_total_pages * PAGE_SIZE - 1));

    uint32_t idx = (uint32_t)vaddr >> 12;
    for (int i = 0; i < count; i++) {
        bitmap_set(&kernel_pool.pool_map, idx + i, false);
    }

    printk("Freed Kernel Pages\n==> Base: %p\n", (void *)vaddr);
}

/* 内存映射 */
void mem_mapping() {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t *)VBE_MODE_INFO_BASE;

    low1MB_pt_counts = 1;
    uint32_t res_size = mode_info->x_resolution * mode_info->y_resolution * 4;
    uint32_t res_pages = CEIL(res_size, PAGE_SIZE);
    fb_pt_counts = CEIL(res_pages, 1024);

    page_dir_entry_t *pd = (page_dir_entry_t *)KERNEL_PAGE_DIR_VADDR;
    page_table_entry_t *pt = (page_table_entry_t *)KERNEL_PAGE_TABLE_VADDR;

    uint32_t kernel_idx = (uint32_t)mem_info.available_base >> 12;
    uint32_t kernel_map_pages = CEIL(KERNEL_RESERVED_SIZE, PAGE_SIZE);
    kernel_pt_counts = CEIL(kernel_map_pages, 1024);
    for (int pd_idx = 768; pd_idx < 768 + kernel_pt_counts; pd_idx++) {
        memset(pt, 0, PAGE_SIZE);
        entry_init(&pd[pd_idx], KERNEL_PADDR(pt) >> 12);

        for (int pt_idx = 0; pt_idx < 1024; pt_idx++, kernel_idx++) {
            entry_init(&pt[pt_idx], kernel_idx);
        }

        pt += 1024;
    }

    fb_page_table_base = KERNEL_PAGE_TABLE_VADDR + kernel_pt_counts * PAGE_SIZE;
    pt = (page_table_entry_t *)fb_page_table_base;
    uint32_t fb_idx = (uint32_t)mode_info->framebuffer >> 12;
    uint32_t fb_pd_idx = (uint32_t)mode_info->framebuffer >> 22;
    for (int pd_idx = fb_pd_idx; pd_idx < fb_pd_idx + fb_pt_counts; pd_idx++) {
        memset(pt, 0, PAGE_SIZE);
        entry_init(&pd[pd_idx], KERNEL_PADDR(pt) >> 12);

        for (int pt_idx = 0; pt_idx < 1024; pt_idx++, fb_idx++) {
            entry_init(&pt[pt_idx], fb_idx);
        }

        pt += 1024;
    }

    printk("Page Table Counts\n==> Low 1MB: %d  Kernel: %d  Frame Buffer: %d\n",
        low1MB_pt_counts,
        kernel_pt_counts,
        fb_pt_counts);
}

/* 内存池初始化 */
void mem_pool_init() {
    phys_pool.pool_base = mem_info.available_base;
    phys_pool.pool_total_pages = mem_info.available_pages;
    phys_pool.pool_available_pages = phys_pool.pool_total_pages;
    phys_pool.pool_map_base = fb_page_table_base + fb_pt_counts * PAGE_SIZE;
    phys_pool.pool_map_size = CEIL(phys_pool.pool_total_pages, 8);
    phys_pool.pool_map_pages = CEIL(phys_pool.pool_map_size, PAGE_SIZE);

    kernel_pool.pool_base = KERNEL_VADDR;
    kernel_pool.pool_total_pages = CEIL(KERNEL_RESERVED_SIZE, PAGE_SIZE);
    kernel_pool.pool_available_pages = kernel_pool.pool_total_pages;
    kernel_pool.pool_map_base = phys_pool.pool_map_base + phys_pool.pool_map_pages * PAGE_SIZE;
    kernel_pool.pool_map_size = CEIL(kernel_pool.pool_total_pages, 8);
    kernel_pool.pool_map_pages = CEIL(kernel_pool.pool_map_size, PAGE_SIZE);

    uint32_t phys_map_start = phys_pool.pool_base >> 12;
    bitmap_init(&phys_pool.pool_map, (void *)phys_pool.pool_map_base, phys_pool.pool_map_size, phys_map_start);

    uint32_t kernel_map_start = kernel_pool.pool_base >> 12;
    bitmap_init(&kernel_pool.pool_map, (void *)kernel_pool.pool_map_base, kernel_pool.pool_map_size, kernel_map_start);

    uint32_t phys_used_pages =
        (MB_SIZE / PAGE_SIZE)           // 内核代码(1MB)
        + 1                             // 内核页目录
        + low1MB_pt_counts              // 低1MB页表
        + kernel_pt_counts              // 内核页表
        + fb_pt_counts                  // 帧缓冲区页表
        + phys_pool.pool_map_pages      // 物理页位图
        + kernel_pool.pool_map_pages;   // 内核虚拟页位图
    
    for (int i = 0; i < phys_used_pages; i++) {
        bitmap_set(&phys_pool.pool_map, phys_map_start + i, true);
        bitmap_set(&kernel_pool.pool_map, kernel_map_start + i, true);
    }
    
    phys_pool.pool_available_pages -= phys_used_pages;
    kernel_pool.pool_available_pages -= phys_used_pages;

    printk("Memory Pool Info\n");
    printk("==> Phys Pool Base: 0x%p  Phys Map Base: 0x%p  Phys Map Size: %d(Pages: %d)  Used Pages: %d\n",
        phys_pool.pool_base,
        phys_pool.pool_map_base,
        phys_pool.pool_map_size,
        phys_pool.pool_map_pages,
        phys_used_pages);
    printk("==> Kernel Pool Base: 0x%p  Kernel Map Base: 0x%p  Kernel Map Size: %d(Pages: %d)  Used Pages: %d\n",
        kernel_pool.pool_base,
        kernel_pool.pool_map_base,
        kernel_pool.pool_map_size,
        kernel_pool.pool_map_pages,
        phys_used_pages);
}

/* 内存管理初始化 */
void mem_init() {
    get_mem_info((void *)MEM_INFO_BLOCK_BASE);

    mem_mapping();
    mem_pool_init();
}