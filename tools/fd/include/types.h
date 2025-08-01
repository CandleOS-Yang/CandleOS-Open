#ifndef __TYPES_H__
#define __TYPES_H__

// 禁止编译器优化字节大小
#define _packed    __attribute__((packed))

// 简写无符号基础 - 类型
typedef unsigned char       u8;         // 无符号 - 1字节
typedef unsigned short      u16;        // 无符号 - 2字节
typedef unsigned int        u32;        // 无符号 - 4字节
typedef unsigned long long  u64;        // 无符号 - 8字节

// 简写有符号基础 - 类型
typedef signed char         i8;         // 有符号 - 1字节
typedef signed short        i16;        // 有符号 - 2字节
typedef signed int          i32;        // 有符号 - 4字节
typedef signed long long    i64;        // 有符号 - 8字节

// 向上取整
#define ceil(n, m) (((n - 1) / m) + 1)

#endif