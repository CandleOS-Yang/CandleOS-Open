#ifndef __IO_H__
#define __IO_H__

#include "stdint.h"

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);
uint32_t inl(uint16_t port);
void outl(uint16_t port, uint32_t value);
uint32_t get_cr0();
void set_cr0(uint32_t cr0);
uint32_t get_cr3();
void set_cr3(uint32_t cr3);
uint32_t get_cr4();
void set_cr4(uint32_t cr4);


#endif
