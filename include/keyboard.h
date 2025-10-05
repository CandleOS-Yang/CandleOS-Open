#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "stdint.h"

// 特殊键扫描码定义
#define KEY_LSHIFT  0x2A
#define KEY_RSHIFT  0x36
#define KEY_CTRL    0x1D
#define KEY_ALT     0x38
#define KEY_ENTER   0x1C
#define KEY_CAPS    0x3A
#define KEY_NUMLOCK 0x45

// 键盘LED位掩码
#define KBD_LED_SCROLL_LOCK  0x01
#define KBD_LED_NUM_LOCK     0x02
#define KBD_LED_CAPS_LOCK    0x04

#define PS2_DATA_PORT 0x60               // PS/2 数据端口
#define PS2_STATUS_PORT 0x64             // PS/2 状态端口
#define PS2_CMD_PORT 0x64                // PS/2 命令端口

#define PS2_WRITE_CMD 0x60               // 写入 PS/2控制器 命令
#define PS2_READ_CMD 0x20                // 读取 PS/2控制器 命令

#define PS2_SET_LEDS 0xed                // 设置键盘LED 配置字节

#define PS2_MOUSE_ACK 0xfa               // 鼠标控制器 ACK

uint8_t scancode_to_ascii(uint8_t scan_code);
void wait_keyboard_ready();
uint8_t read_keyboard_leds();
void keyboard_isr_handler(uint32_t vector);
void keyboard_init();

#endif