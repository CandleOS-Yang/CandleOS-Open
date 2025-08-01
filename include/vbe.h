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
    uint32_t *frame_buffer;          // 帧缓冲区基地址
} VbeModeInfo_t;

#endif