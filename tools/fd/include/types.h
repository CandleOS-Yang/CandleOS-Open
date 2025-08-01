#ifndef __TYPES_H__
#define __TYPES_H__

// ��ֹ�������Ż��ֽڴ�С
#define _packed    __attribute__((packed))

// ��д�޷��Ż��� - ����
typedef unsigned char       u8;         // �޷��� - 1�ֽ�
typedef unsigned short      u16;        // �޷��� - 2�ֽ�
typedef unsigned int        u32;        // �޷��� - 4�ֽ�
typedef unsigned long long  u64;        // �޷��� - 8�ֽ�

// ��д�з��Ż��� - ����
typedef signed char         i8;         // �з��� - 1�ֽ�
typedef signed short        i16;        // �з��� - 2�ֽ�
typedef signed int          i32;        // �з��� - 4�ֽ�
typedef signed long long    i64;        // �з��� - 8�ֽ�

// ����ȡ��
#define ceil(n, m) (((n - 1) / m) + 1)

#endif