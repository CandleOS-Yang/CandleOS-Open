#include "stdint.h"
#include "vbe.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "printk.h"
#include "string.h"

extern char kfont[4096];                // SongFonts字体库
#define CHAR_HEIGHT 16                  // 字符高度
#define CHAR_WIDTH 8                    // 字符宽度
uint32_t current_cursor_x;              // 当前光标水平位置
uint32_t current_cursor_y;              // 当前光标垂直位置

/* 清理屏幕 */
void vbe_clear(VbeModeInfo_t *mode_info) {
    uint32_t *fb = (uint32_t *)mode_info->framebuffer;
    for (int i = 0; i < mode_info->x_resolution * mode_info->y_resolution; i++) {
        fb[i] = 0xff000000;
    }
}

/* 滚屏 */
void vbe_scroll(VbeModeInfo_t *mode_info) {
    if (current_cursor_y >= mode_info->y_resolution / CHAR_HEIGHT) {
        void *src = (void *)&mode_info->framebuffer[CHAR_HEIGHT * mode_info->x_resolution];
        void *dst = (void *)mode_info->framebuffer;
        uint32_t scroll_size = mode_info->x_resolution * (mode_info->y_resolution - CHAR_HEIGHT) * 4;

        // memcpy(dst, src, scroll_size);

        for (uint32_t i = 0; i < scroll_size; i += 64) {
            asm volatile (
                "movdqu xmm0,[%0]\n"
                "movdqu xmm1,[%0 + 16]\n"
                "movdqu xmm2,[%0 + 32]\n"
                "movdqu xmm3,[%0 + 48]\n"
                
                "movdqu [%1],xmm0\n"
                "movdqu [%1 + 16],xmm1\n"
                "movdqu [%1 + 32],xmm2\n"
                "movdqu [%1 + 48],xmm3\n"
                : 
                : "r" (src + i), "r"(dst + i)
                : "xmm0", "xmm1", "xmm2", "xmm3", "memory"
            );
        }

        void *clear_src = (void *)dst + scroll_size;
        uint32_t clear_size = mode_info->x_resolution * CHAR_HEIGHT * 4;

        // memset(clear_src, 0, clear_size);

        asm volatile(
            "pxor xmm0, xmm0\n"
            "pxor xmm1, xmm1\n"
            "pxor xmm2, xmm2\n"
            "pxor xmm3, xmm3\n"
        );
        for (uint32_t i = 0; i < clear_size; i += 64) {
            asm volatile (
                "movdqu [%0],xmm0\n"
                "movdqu [%0 + 16],xmm1\n"
                "movdqu [%0 + 32],xmm2\n"
                "movdqu [%0 + 48],xmm3\n"
                : 
                : "r" (clear_src + i)
            );
        }
    
        current_cursor_y = mode_info->y_resolution / CHAR_HEIGHT - 1;
    }
}

/* 光标前进 */
void advance_cursor(VbeModeInfo_t *mode_info) {
    int max_chars_per_line = mode_info->x_resolution / CHAR_WIDTH;
    current_cursor_x++;
    
    if (current_cursor_x >= max_chars_per_line) {
        current_cursor_x = 0;
        current_cursor_y++;

        if (current_cursor_y >= mode_info->y_resolution / CHAR_HEIGHT) {
            vbe_scroll(mode_info);
        }
    }
}

/* 绘制字符 */
void vbe_draw_char(VbeModeInfo_t *mode_info, uint32_t x, uint32_t y, uint32_t color, char *ch) {
    for (size_t i = 0; i < 16; i++) {
        for (size_t j = 0; j < 8; j++) {
            if (ch[i] & (0x80 >> j)) {
                mode_info->framebuffer[(y + i) * mode_info->x_resolution + (x + j)] = color;
            }
        }
    }
}

/* 输出字符 */
void vbe_put_char(VbeModeInfo_t *mode_info, uint32_t color, char ch) {
    if (ch == '\n') {
        current_cursor_x = 0;
        current_cursor_y++;
        vbe_scroll(mode_info);
        return;
    }

    vbe_draw_char(mode_info, current_cursor_x * 8, current_cursor_y * 16, color, kfont + (ch * 16));
    advance_cursor(mode_info);
}

/* 输出字符串 */
void vbe_put_string(VbeModeInfo_t *mode_info, uint32_t color, char *str) {
    while (*str != EOS) {
        vbe_put_char(mode_info, color, *str);
        str++;
    }
}

/* VBE初始化 */
void vbe_init(VbeModeInfo_t *mode_info) {
    current_cursor_x = 0;
    current_cursor_y = 0;
    vbe_clear(mode_info);

    printk("VBE Mode Info:\n==> FrameBuffer: 0x%p  X Resolution: %d  Y Resolution: %d\n",
        mode_info->framebuffer,
        mode_info->x_resolution,
        mode_info->y_resolution);
}