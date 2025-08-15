#include "stdint.h"
#include "heap.h"
#include "mem.h"
#include "debug.h"

arena_descriptor_t global_descriptors[8];        // 不同大小的空闲块链表

/* 初始化内存块链表 */
void mem_block_list_init(mem_block_list_t *list) {
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;
}

/* 添加内存块 */
void add_mem_block(mem_block_list_t *list, mem_block_t *block) {
    block->next = list->head.next;
    block->prev = &list->head;

    list->head.next->prev = block;
    list->head.next = block;
}

/* 去除内存块 */
void remove_mem_block(mem_block_list_t *list, mem_block_t *block) {
    block->prev->next = block->next;
    block->next->prev = block->prev;

    block->next = NULL;
    block->prev = NULL;
}

/* 获取内存块 */
mem_block_t *get_mem_block(mem_block_list_t *list) {
    mem_block_t *block = list->head.next;
    remove_mem_block(list, block);
    return block;
}

/* 判断内存块链表是否为空 */
bool mem_block_list_empty(mem_block_list_t *list) {
    return (list->head.next == &list->tail);
}

/* 查找指定节点 */
bool search_block(mem_block_list_t *list, mem_block_t *node) {
    mem_block_t *next = list->head.next;

    while (next != &list->tail) {
        if (next == node)
            return true;
        next = next->next;
    }
    return false;
}

/* 获取idx的内存块地址 */
mem_block_t *get_block_address(arena_t *arena, uint32_t idx) {
    void *base = (void *)(arena + 1);
    uint32_t offset = idx * arena->descriptor->block_size;

    return (mem_block_t *)(base + offset);
}

/* 获取arena地址 */
arena_t *get_arena_address(mem_block_t *block) {
    return (arena_t *)((uint32_t)block & 0xfffff000);
}

/* 初始化堆管理 */
void heap_init() {
    uint32_t block_size = 16;
    for (int i = 0; i < 8; i++) {
        arena_descriptor_t *descriptor = &global_descriptors[i];
        descriptor->block_size = block_size;
        descriptor->total_blocks = (PAGE_SIZE - sizeof(arena_t)) / block_size;
        mem_block_list_init(&descriptor->free_list);

        block_size *= 2;
    }
}

/* 分配内核堆内存 */
void *kmalloc(size_t size) {
    assert(size > 0);
    
    arena_t *arena = NULL;
    arena_descriptor_t *descriptor = NULL;
    mem_block_t *block;

    if (size > 1024) {
        uint32_t total_size = size + sizeof(arena_t);
        uint32_t total_pages = CEIL(total_size, PAGE_SIZE);
        arena = alloc_kernel_pages(total_pages);
        assert(arena != NULL);

        arena->descriptor = NULL;
        arena->free_blocks = total_pages;
        arena->is_large = true;
        return (void *)((uint32_t)arena + sizeof(arena_t));
    }

    for (int i = 0; i < 8; i++) {
        if (size <= global_descriptors[i].block_size) {
            descriptor = &global_descriptors[i];
            break;
        }
    }

    if (mem_block_list_empty(&descriptor->free_list)) {
        arena = alloc_kernel_pages(1);
        assert(arena != NULL);

        arena->descriptor = descriptor;
        arena->free_blocks = descriptor->total_blocks;
        arena->is_large = false;

        for (int i = 0; i < descriptor->total_blocks; i++) {
            block = get_block_address(arena, i);
            assert(!search_block(&descriptor->free_list, block));
            add_mem_block(&descriptor->free_list, block);
            assert(search_block(&descriptor->free_list, block));
        }
    }

    block = get_mem_block(&descriptor->free_list);
    arena = get_arena_address(block);

    arena->free_blocks--;
    return (void *)block;
}

/* 释放内核堆内存 */
void kfree(void *ptr) {
    assert(ptr != NULL);

    mem_block_t *block = (mem_block_t *)ptr;
    arena_t *arena = get_arena_address(block);

    if (arena->is_large) {
        free_kernel_pages(arena, arena->free_blocks);
        return;
    }

    add_mem_block(&arena->descriptor->free_list, block);
    arena->free_blocks++;

    if (arena->free_blocks == arena->descriptor->total_blocks) {
        for (int i = 0; i < arena->descriptor->total_blocks; i++) {
            mem_block_t *block = get_block_address(arena, i);
            assert(search_block(&arena->descriptor->free_list, block));
            remove_mem_block(&arena->descriptor->free_list, block);
            assert(!search_block(&arena->descriptor->free_list, block));
        }
        
        free_kernel_pages(arena, 1);
    }
}