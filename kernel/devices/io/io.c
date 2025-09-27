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