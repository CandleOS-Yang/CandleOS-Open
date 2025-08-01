#ifndef __STDINT_H__
#define __STDINT_H__

// 禁止编译器优化字节大小
#define _packed    __attribute__((packed))
#define _alogned8  __attribute__((aligned(8)))
#define _alogned16 __attribute__((aligned(16)))
#define _alogned32 __attribute__((aligned(32)))

// 空 - 类型
#define NULL ((void *)0)

// 文件结束类型
#define EOF -1

// 字符串结尾
#define EOS '\0'

#define bool  _Bool
#define True  1
#define true  1
#define False 0
#define false 0

typedef unsigned char       uint8_t;    // 无符号 - 1字节
typedef unsigned short      uint16_t;   // 无符号 - 2字节
typedef unsigned int        uint32_t;   // 无符号 - 4字节
typedef unsigned long long  uint64_t;   // 无符号 - 8字节

typedef signed char         int8_t;     // 有符号 - 1字节
typedef signed short        int16_t;    // 有符号 - 2字节
typedef signed int          int32_t;    // 有符号 - 4字节
typedef signed long long    int64_t;    // 有符号 - 8字节

typedef uint32_t size_t;

#endif