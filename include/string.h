#ifndef __STRING_H__
#define __STRING_H__

#include "./stdint.h"

int8_t *strcpy(int8_t *dest, const int8_t *src);
int8_t *strncpy(int8_t *dest, const int8_t *src, size_t count);
int8_t *strcat(int8_t *dest, const int8_t *src);
size_t strlen(const int8_t *str);
size_t strnlen(const int8_t *str, size_t maxlen);
int strcmp(const int8_t *lhs, const int8_t *rhs);
int8_t *strchr(const int8_t *str, int ch);
int8_t *strrchr(const int8_t *str, int ch);
int8_t *strsep(const int8_t *str);
int8_t *strrsep(const int8_t *str);
int memcmp(const void *lhs, const void *rhs, size_t count);
void *memset(void *dest, int ch, size_t count);
void *memcpy(void *dest, const void *src, size_t count);
void *memchr(const void *ptr, int ch, size_t count);
char* strupr(char* src);

#endif