#ifndef __PIC_H__
#define __PIC_H__

#include "./stdint.h"

#define PIC_CMD_PRIMARY 0x0020     // PIC 主片命令端口
#define PIC_CMD_SECONDARY 0x00A0   // PIC 从片命令端口

#define PIC_DATA_PRIMARY 0x0021    // PIC 主片数据端口
#define PIC_DATA_SECONDARY 0x00A1  // PIC 从片数据端口

#define PIC_ICW1 0x11              // 通用  初始化命令字1
#define PIC_ICW3_PRIMARY 0x04      // 主片 初始化命令字3
#define PIC_ICW3_SECONDARY 0x02    // 从片 初始化命令字3
#define PIC_ICW4 0x01              // 通用  初始化命令字4

#define PIC_OCW1_PRIMARY 0xfb      // 主片  操作命令字1
#define PIC_OCW1_SECONDARY 0xff    // 从片  操作命令字1
#define PIC_OCW2 0x20              // 通用  操作命令字2

#define PIC_EOI 0x20               // 中断结束命令

#define IRQ_PRIMARY_OFFSET 0x20    // 主片起始中断向量
#define IRQ_SECONDARY_OFFSET 0x28  // 从片起始中断向量

#endif