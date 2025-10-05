#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "stdint.h"

#define PS2_MOUSE_WRITE_CMD 0xd4         // 写入 PS/2鼠标控制器 命令

#define PS2_ENBLE_MOUSE_INTR 0x47        // 启用鼠标中断 配置字节
#define PS2_ENBLE_MOUSE_REPORTING 0xf4   // 启用鼠标数据传输 配置字节

// 鼠标信息结构
typedef struct _packed {
    int8_t data[3];                      // 鼠标原始数据
    uint8_t status;                      // 鼠标解包状态
    int32_t x_offset;                    // 鼠标x偏移
    int32_t y_offset;                    // 鼠标y偏移
    int32_t x;                           // 鼠标x坐标
    int32_t y;                           // 鼠标y坐标
    bool left_btn;                       // 鼠标左键状态
    bool middle_btn;                     // 鼠标中键状态
    bool right_btn;                      // 鼠标右键状态
} mouse_info_t;

void mouse_isr_handler(uint32_t vector);
void enable_mouse();

#endif