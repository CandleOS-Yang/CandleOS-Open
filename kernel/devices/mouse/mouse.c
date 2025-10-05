#include "mouse.h"
#include "keyboard.h"
#include "stdint.h"
#include "printk.h"
#include "debug.h"
#include "io.h"
#include "interrupt.h"
#include "idt.h"

extern mouse_info_t mouse_info;        // 鼠标信息

/* 鼠标数据包解码 */
bool ps2_mouse_decode(mouse_info_t *mouse_info, uint8_t data) {
    if (mouse_info->status == 0) {
        if (data == PS2_MOUSE_ACK) {
            mouse_info->status = 1;
        }
        return false;
    }
    if (mouse_info->status == 1) {
        mouse_info->data[0] = data;
        mouse_info->status = 2;
        return false;
    }
    if (mouse_info->status == 2) {
        mouse_info->data[1] = data;
        mouse_info->status = 3;
        return false;
    }
    if (mouse_info->status == 3) {
        mouse_info->data[2] = data;
        mouse_info->status = 1;

        mouse_info->left_btn = (mouse_info->data[0] & 0x01) ? 1 : 0;
        mouse_info->right_btn = (mouse_info->data[0] & 0x02) ? 1 : 0;
        mouse_info->middle_btn = (mouse_info->data[0] & 0x04) ? 1 : 0;

        mouse_info->x_offset = mouse_info->data[1];
        mouse_info->y_offset = mouse_info->data[2];

        if ((mouse_info->data[0] & 0x10) != 0) {
			mouse_info->x_offset |= 0xffffff00;
		}
		if ((mouse_info->data[0] & 0x20) != 0) {
			mouse_info->y_offset |= 0xffffff00;
		}

        mouse_info->y_offset = -mouse_info->y_offset;

        mouse_info->x += mouse_info->x_offset;
        mouse_info->y += mouse_info->y_offset;
        return true;
    }
    return false;
}

/* 鼠标ISR处理函数 */
void mouse_isr_handler(uint32_t vector) {
    uint8_t mouse_data = inb(0x60);
    ps2_mouse_decode(&mouse_info, mouse_data);

    printk("Mouse: x=%d, y=%d, left=%d, middle=%d, right=%d\n", 
        mouse_info.x, mouse_info.y, 
        mouse_info.left_btn, mouse_info.middle_btn, mouse_info.right_btn);
    send_eoi(vector);
}

/* 开启鼠标 */
void enable_mouse() {
    set_isr_handler(0x2c, (uint32_t)mouse_isr_handler);
    enable_int_bit(0x20 + 2);
    enable_int_bit(0x28 + 1);
    enable_int_bit(0x28 + 4);
    
    // 开启鼠标中断
    wait_keyboard_ready();
    outb(PS2_CMD_PORT, PS2_WRITE_CMD);
    wait_keyboard_ready();
    outb(PS2_DATA_PORT, PS2_ENBLE_MOUSE_INTR);

    // 启用鼠标数据传输
    wait_keyboard_ready();
    outb(PS2_CMD_PORT, PS2_MOUSE_WRITE_CMD);
    wait_keyboard_ready();
    outb(PS2_DATA_PORT, PS2_ENBLE_MOUSE_REPORTING);
}

/* 初始化鼠标 */
void mouse_init(mouse_info_t *mouse_info) {
    memset(mouse_info, 0, sizeof(mouse_info_t));
    
    enable_mouse();
}