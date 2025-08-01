#include "fat16.h"
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* ×ª»»×Ö·û´®´óÐ´ */
char* strupr(char* src) {
	while (*src != '\0') {
		if (*src >= 'a' && *src <= 'z') *src -= 32;
		src++;
	}
	return src;
}