#ifndef __MEM_H__
#define __MEM_H__

#include "stdint.h"
#include "bitmap.h"

#define MEM_INFO_BLOCK_BASE 0xA000       // 内存信息块基址
#define PHYS_MAP_BASE 0x200000           // 物理页内存池位图基址

#define ARDS_TYPE_AVAILABLE 1            // 可用内存

#define PAGE_SIZE 4096                   // 页大小
#define MB_SIZE (1024 * 1024)            // MB大小

#define KERNEL_PT_COUNTS 4               // 内核页表数量(4 * 4MB = 16MB)
#define KERNEL_PDE_COUNTS 4              // 内核页目录项数量

#define VADDR(pd_idx, pt_idx, off) (uint32_t)((pd_idx << 22) | (pt_idx << 12) | off)   // 获取虚拟地址

typedef struct _packed {
    uint64_t base;                       // 基址
    uint64_t size;                       // 大小
    uint32_t type;                       // 类型
} Ards_t;

typedef struct _packed {
    uint32_t total_size;                 // 总大小(Bytes)
    uint32_t total_pages;                // 总页数
    uint32_t available_base;             // 可用内存基址
    uint32_t available_size;             // 可用大小(Bytes)
    uint32_t available_pages;            // 可用页数
} MemoryInfo_t;

typedef struct _packed {
    uint32_t pool_total_pages;           // 内存池总页数
    uint32_t pool_available_pages;       // 内存池可用页数
    bitmap_t map;                        // 位图
    uint32_t map_base;                   // 位图基址
    uint32_t map_size;                   // 位图大小(Bytes)
} MemoryPool_t;

typedef struct {
    uint32_t page_dir_idx;               // 页目录索引号
    uint32_t page_table_idx;             // 页表索引号
    uint32_t page_offset;                // 页内偏移
} vaddr_info_t;

typedef struct _packed {
    uint8_t present : 1;                 // 存在位     (1：内存中 0：磁盘上)
    uint8_t write : 1;                   // 读写权限   (0：只读 1：可读可写)
    uint8_t user : 1;                    // 用户权限   (1：所有人 0：超级用户)
    uint8_t pwt : 1;                     // 页写模式   (1:直写模式；0:回写模式)
    uint8_t pcd : 1;                     // 禁止页缓冲
    uint8_t accessed : 1;                // 访问位
    uint8_t dirty : 1;                   // 脏页(页缓冲被写过)
    uint8_t pat : 1;                     // 页大小     (4K / 4M)
    uint8_t global : 1;                  // 全局位
    uint8_t ignored : 3;                 // 可用位
    uint32_t index : 20;                 // 页项索引
} entry_t;

typedef entry_t page_table_entry_t;      // 页表项
typedef entry_t page_dir_entry_t;        // 页目录项

#endif