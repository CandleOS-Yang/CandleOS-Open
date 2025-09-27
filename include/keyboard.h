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

void keyboard_isr_handler(uint32_t vector);

#endif