#ifndef __HEAP_H__
#define __HEAP_H__

#include "stdint.h"

typedef struct mem_block {
    struct mem_block *prev;            // 前一个内存块
    struct mem_block *next;            // 后一个内存块
} mem_block_t;

typedef struct {
    mem_block_t head;                   // 内存块链表头
    mem_block_t tail;                   // 内存块链表尾
} mem_block_list_t;

typedef struct {
    uint32_t block_size;                // 内存块大小
    uint32_t total_blocks;              // 总内存块数量
    mem_block_list_t free_list;         // 空闲内存块链表
} arena_descriptor_t;

typedef struct {
    arena_descriptor_t *descriptor;     // 所属的描述符
    uint32_t free_blocks;               // 空闲内存块数量
    bool is_large;                      // 是否是大块内存(0：小块  1：大块)
} arena_t;


void mem_block_list_init(mem_block_list_t *list);
void add_mem_block(mem_block_list_t *list, mem_block_t *block);
void remove_mem_block(mem_block_list_t *list, mem_block_t *block);
mem_block_t *get_mem_block(mem_block_list_t *list);
bool mem_block_list_empty(mem_block_list_t *list);
bool search_block(mem_block_list_t *list, mem_block_t *node);
mem_block_t *get_block_address(arena_t *arena, uint32_t idx);
arena_t *get_arena_address(mem_block_t *block);
void heap_init();
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif