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
    uint32_t *frame_buffer;          // ֡����������ַ
} VbeModeInfo_t;

#endif