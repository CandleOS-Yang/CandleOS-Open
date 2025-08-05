#ifndef _VBE_H_
#define _VBE_H_

#include "stdint.h"

#define VBE_MODE_INFO_BASE 0xA200    // VBE模式信息基地址

typedef struct _packed {
    uint16_t mode;                   // 模式号
    uint16_t version;                // VBE版本号
    uint16_t bpp;                    // 每像素占用位宽
    uint16_t x_resolution;           // 水平分辨率
    uint16_t y_resolution;           // 垂直分辨率
    uint32_t *framebuffer;           // 帧缓冲区基地址
} VbeModeInfo_t;

void vbe_clear(VbeModeInfo_t *mode_info);
void vbe_scroll(VbeModeInfo_t *mode_info);
void vbe_draw_char(VbeModeInfo_t *mode_info, uint32_t x, uint32_t y, uint32_t color, char *ch);
void vbe_put_char(VbeModeInfo_t *mode_info, uint32_t color, char ch);
void vbe_put_string(VbeModeInfo_t *mode_info, uint32_t color, char *str);
void vbe_init(VbeModeInfo_t *mode_info);

#endif