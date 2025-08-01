#ifndef __STDINT_H__
#define __STDINT_H__

// ��ֹ�������Ż��ֽڴ�С
#define _packed    __attribute__((packed))
#define _alogned8  __attribute__((aligned(8)))
#define _alogned16 __attribute__((aligned(16)))
#define _alogned32 __attribute__((aligned(32)))

// �� - ����
#define NULL ((void *)0)

// �ļ���������
#define EOF -1

// �ַ�����β
#define EOS '\0'

#define bool  _Bool
#define True  1
#define true  1
#define False 0
#define false 0

typedef unsigned char       uint8_t;    // �޷��� - 1�ֽ�
typedef unsigned short      uint16_t;   // �޷��� - 2�ֽ�
typedef unsigned int        uint32_t;   // �޷��� - 4�ֽ�
typedef unsigned long long  uint64_t;   // �޷��� - 8�ֽ�

typedef signed char         int8_t;     // �з��� - 1�ֽ�
typedef signed short        int16_t;    // �з��� - 2�ֽ�
typedef signed int          int32_t;    // �з��� - 4�ֽ�
typedef signed long long    int64_t;    // �з��� - 8�ֽ�

typedef uint32_t size_t;

#endif