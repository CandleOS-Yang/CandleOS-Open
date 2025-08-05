#ifndef _VBE_H_
#define _VBE_H_

#include "stdint.h"

#define VBE_MODE_INFO_BASE 0xA200    // VBEģʽ��Ϣ����ַ

typedef struct _packed {
    uint16_t mode;                   // ģʽ��
    uint16_t version;                // VBE�汾��
    uint16_t bpp;                    // ÿ����ռ��λ��
    uint16_t x_resolution;           // ˮƽ�ֱ���
    uint16_t y_resolution;           // ��ֱ�ֱ���
    uint32_t *framebuffer;           // ֡����������ַ
} VbeModeInfo_t;

void vbe_clear(VbeModeInfo_t *mode_info);
void vbe_scroll(VbeModeInfo_t *mode_info);
void vbe_draw_char(VbeModeInfo_t *mode_info, uint32_t x, uint32_t y, uint32_t color, char *ch);
void vbe_put_char(VbeModeInfo_t *mode_info, uint32_t color, char ch);
void vbe_put_string(VbeModeInfo_t *mode_info, uint32_t color, char *str);
void vbe_init(VbeModeInfo_t *mode_info);

#endif