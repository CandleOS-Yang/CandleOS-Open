#include "io.h"
#include "stdint.h"

/* 读端口 (8位) */
uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile(
        "in %0,%1"
        : "=a" (value)
        : "d" (port)
    );
    return value;
}

/* 写端口 (8位)  */
void outb(uint16_t port, uint8_t value) {
    asm volatile (
        "out %1,%0"
        :
        : "a" (value), "d" (port)
    );
}

/* 读端口 (16位) */
uint16_t inw(uint16_t port) {
    uint16_t value;
    asm volatile(
        "in %0,%1"
        : "=a" (value)
        : "d" (port)
    );
    return value;
}

/* 写端口 (16位) */
void outw(uint16_t port, uint16_t value) {
    asm volatile (
        "out %1,%0"
        :
        : "a" (value), "d" (port)
    );
}

/* 读端口 (32位) */
uint32_t inl(uint16_t port) {
    uint32_t value;
    asm volatile(
        "in %0,%1"
        : "=a" (value)
        : "d" (port)
    );
    return value;
}

/* 写端口 (32位) */
void outl(uint16_t port, uint32_t value) {
    asm volatile (
        "out %1,%0"
        :
        : "a" (value), "d" (port)
    );
}

/* 获取CR0值 */
uint32_t get_cr0() {
    uint32_t cr0;
    __asm__ volatile("mov %0,cr0" : "=r"(cr0));
    return cr0;
}

/* 设置CR0值 */
void set_cr0(uint32_t cr0) {
    __asm__ volatile("mov cr0,%0" : : "r"(cr0));
}

/* 获取CR3值 */
uint32_t get_cr3(void) {
    uint32_t cr3;
    __asm__ volatile("mov %0,cr3" : "=r"(cr3));
    return cr3;
}

/* 设置CR3值 */
void set_cr3(uint32_t cr3) {
    __asm__ volatile("mov cr3,%0" : : "r"(cr3));
}

/* 获取CR4值 */
uint32_t get_cr4(void) {
    uint32_t cr4;
    __asm__ volatile("mov %0,cr4" : "=r"(cr4));
    return cr4;
}

/* 设置CR4值 */
void set_cr4(uint32_t cr4) {
    __asm__ volatile("mov cr4,%0" : : "r"(cr4));
}