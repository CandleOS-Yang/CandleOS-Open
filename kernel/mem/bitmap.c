#include "bitmap.h"
#include "stdint.h"
#include "string.h"
#include "debug.h"

/* 位图初始化 */
void bitmap_init(bitmap_t *bitmap, uint8_t *buf, uint32_t size, uint32_t off) {
    bitmap->buf = buf;
    bitmap->size = size;
    bitmap->off = off;

    memset(bitmap->buf, 0, size);
}

/* 检测位图指定位的数值 */
bool bitmap_test(bitmap_t *bitmap, uint32_t idx) {
    assert(idx >= bitmap->off);

    uint32_t index = idx - bitmap->off;
    uint32_t byte = index / 8;
    uint32_t bit = index % 8;

    assert(byte < bitmap->size);
    return (bitmap->buf[byte] & (1 << bit));
}

/* 设置位图指定位的数值 */
void bitmap_set(bitmap_t *bitmap, uint32_t idx, uint32_t value) {
    assert(value == 0 || value == 1);
    assert(idx >= bitmap->off);

    uint32_t index = idx - bitmap->off;
    uint32_t byte = index / 8;
    uint32_t bit = index % 8;

    assert(byte < bitmap->size);

    if (value) {
        bitmap->buf[byte] |= (1 << bit);
    } else {
        bitmap->buf[byte] &= ~(1 << bit);
    }
}

/* 查找位图空闲位 */
uint32_t bitmap_scan(bitmap_t *bitmap, uint32_t cnt) {
    uint32_t start = -1;
    uint32_t count = 0;
    uint32_t next_bit = 0;
    uint32_t bits = bitmap->size * 8;

    while (bits-- > 0) {
        if (!bitmap_test(bitmap, bitmap->off + next_bit)) {
            count++;
        } else {
            count = 0;
        }

        next_bit++;

        if (count == cnt) {
            start = bitmap->off + (next_bit - cnt);
            break;
        }
    }

    if (start == -1) {
        return -1;
    }

    return start;
}