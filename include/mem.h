#ifndef __MEM_H__
#define __MEM_H__

#include "stdint.h"
#include "bitmap.h"

#define MEM_INFO_BLOCK_BASE 0xA000       // �ڴ���Ϣ���ַ
#define PHYS_MAP_BASE 0x200000           // ����ҳ�ڴ��λͼ��ַ

#define ARDS_TYPE_AVAILABLE 1            // �����ڴ�

#define PAGE_SIZE 4096                   // ҳ��С
#define MB_SIZE (1024 * 1024)            // MB��С

#define KERNEL_PT_COUNTS 4               // �ں�ҳ������(4 * 4MB = 16MB)
#define KERNEL_PDE_COUNTS 4              // �ں�ҳĿ¼������

#define VADDR(pd_idx, pt_idx, off) (uint32_t)((pd_idx << 22) | (pt_idx << 12) | off)   // ��ȡ�����ַ

typedef struct _packed {
    uint64_t base;                       // ��ַ
    uint64_t size;                       // ��С
    uint32_t type;                       // ����
} Ards_t;

typedef struct _packed {
    uint32_t total_size;                 // �ܴ�С(Bytes)
    uint32_t total_pages;                // ��ҳ��
    uint32_t available_base;             // �����ڴ��ַ
    uint32_t available_size;             // ���ô�С(Bytes)
    uint32_t available_pages;            // ����ҳ��
} MemoryInfo_t;

typedef struct _packed {
    uint32_t pool_total_pages;           // �ڴ����ҳ��
    uint32_t pool_available_pages;       // �ڴ�ؿ���ҳ��
    bitmap_t map;                        // λͼ
    uint32_t map_base;                   // λͼ��ַ
    uint32_t map_size;                   // λͼ��С(Bytes)
} MemoryPool_t;

typedef struct {
    uint32_t page_dir_idx;               // ҳĿ¼������
    uint32_t page_table_idx;             // ҳ��������
    uint32_t page_offset;                // ҳ��ƫ��
} vaddr_info_t;

typedef struct _packed {
    uint8_t present : 1;                 // ����λ     (1���ڴ��� 0��������)
    uint8_t write : 1;                   // ��дȨ��   (0��ֻ�� 1���ɶ���д)
    uint8_t user : 1;                    // �û�Ȩ��   (1�������� 0�������û�)
    uint8_t pwt : 1;                     // ҳдģʽ   (1:ֱдģʽ��0:��дģʽ)
    uint8_t pcd : 1;                     // ��ֹҳ����
    uint8_t accessed : 1;                // ����λ
    uint8_t dirty : 1;                   // ��ҳ(ҳ���屻д��)
    uint8_t pat : 1;                     // ҳ��С     (4K / 4M)
    uint8_t global : 1;                  // ȫ��λ
    uint8_t ignored : 3;                 // ����λ
    uint32_t index : 20;                 // ҳ������
} entry_t;

typedef entry_t page_table_entry_t;      // ҳ����
typedef entry_t page_dir_entry_t;        // ҳĿ¼��

#endif