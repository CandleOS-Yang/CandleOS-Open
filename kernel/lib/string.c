#include "stdint.h"

int8_t *strcpy(int8_t *dest, const int8_t *src)
{
    int8_t *ptr = dest;
    while (true)
    {
        *ptr++ = *src;
        if (*src++ == EOS)
            return dest;
    }
}

int8_t *strncpy(int8_t *dest, const int8_t *src, size_t count)
{
    int8_t *ptr = dest;
    size_t nr = 0;
    for (; nr < count; nr++)
    {
        *ptr++ = *src;
        if (*src++ == EOS)
            return dest;
    }
    dest[count - 1] = EOS;
    return dest;
}

int8_t *strcat(int8_t *dest, const int8_t *src)
{
    int8_t *ptr = dest;
    while (*ptr != EOS)
    {
        ptr++;
    }
    while (true)
    {
        *ptr++ = *src;
        if (*src++ == EOS)
        {
            return dest;
        }
    }
}

size_t strnlen(const int8_t *str, size_t maxlen)
{
    int8_t *ptr = (int8_t *)str;
    while (*ptr != EOS && maxlen--)
    {
        ptr++;
    }
    return ptr - str;
}

size_t strlen(const int8_t *str)
{
    int8_t *ptr = (int8_t *)str;
    while (*ptr != EOS)
    {
        ptr++;
    }
    return ptr - str;
}

int strcmp(const int8_t *lhs, const int8_t *rhs)
{
    while (*lhs == *rhs && *lhs != EOS && *rhs != EOS)
    {
        lhs++;
        rhs++;
    }
    return *lhs < *rhs ? -1 : *lhs > *rhs;
}

int8_t *strchr(const int8_t *str, int ch)
{
    int8_t *ptr = (int8_t *)str;
    while (true)
    {
        if (*ptr == ch)
        {
            return ptr;
        }
        if (*ptr++ == EOS)
        {
            return NULL;
        }
    }
}

int8_t *strrchr(const int8_t *str, int ch)
{
    int8_t *last = NULL;
    int8_t *ptr = (int8_t *)str;
    while (true)
    {
        if (*ptr == ch)
        {
            last = ptr;
        }
        if (*ptr++ == EOS)
        {
            return last;
        }
    }
}

int memcmp(const void *lhs, const void *rhs, size_t count)
{
    int8_t *lptr = (int8_t *)lhs;
    int8_t *rptr = (int8_t *)rhs;
    while ((count > 0) && *lptr == *rptr)
    {
        lptr++;
        rptr++;
        count--;
    }
    if (count == 0)
        return 0;
    return *lptr < *rptr ? -1 : *lptr > *rptr;
}

void *memset(void *dest, int ch, size_t count)
{
    int8_t *ptr = dest;
    while (count--)
    {
        *ptr++ = ch;
    }
    return dest;
}

void *memcpy(void *dest, const void *src, size_t count)
{
    int8_t *ptr = dest;
    while (count--)
    {
        *ptr++ = *((int8_t *)(src++));
    }
    return dest;
}

void *memchr(const void *str, int ch, size_t count)
{
    int8_t *ptr = (int8_t *)str;
    while (count--)
    {
        if (*ptr == ch)
        {
            return (void *)ptr;
        }
        ptr++;
    }
}

#define SEPARATOR1 '/'                                       // 目录分隔符 1
#define SEPARATOR2 '\\'                                      // 目录分隔符 2
#define IS_SEPARATOR(c) (c == SEPARATOR1 || c == SEPARATOR2) // 字符是否位目录分隔符

// 获取第一个分隔符
int8_t *strsep(const int8_t *str)
{
    int8_t *ptr = (int8_t *)str;
    while (true)
    {
        if (IS_SEPARATOR(*ptr))
        {
            return ptr;
        }
        if (*ptr++ == EOS)
        {
            return NULL;
        }
    }
}

// 获取最后一个分隔符
int8_t *strrsep(const int8_t *str)
{
    int8_t *last = NULL;
    int8_t *ptr = (int8_t *)str;
    while (true)
    {
        if (IS_SEPARATOR(*ptr))
        {
            last = ptr;
        }
        if (*ptr++ == EOS)
        {
            return last;
        }
    }
}

// 字符串转大写
char* strupr(char* src)
{
	while (*src != '\0')
	{
		if (*src >= 'a' && *src <= 'z')
			*src -= 32;
		src++;
	}
	return src;
}