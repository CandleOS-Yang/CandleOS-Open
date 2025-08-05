#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "stdint.h"

typedef struct {
    uint8_t *buf;           // ������
    uint32_t size;          // ��������С
    uint32_t off;           // ��ʼƫ����
} bitmap_t;

void bitmap_init(bitmap_t *bitmap, uint8_t *buf, uint32_t size, uint32_t off);
bool bitmap_test(bitmap_t *bitmap, uint32_t idx);
void bitmap_set(bitmap_t *bitmap, uint32_t idx, uint32_t value);
uint32_t bitmap_scan(bitmap_t *bitmap, uint32_t cnt);

#endif