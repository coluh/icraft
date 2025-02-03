#ifndef _ICRAFT_util_mem_h
#define _ICRAFT_util_mem_h

#include <stdlib.h>
#include <string.h>
#include "log.h"

static inline void *zalloc(int nmemb, int size) {
	void *p = calloc(nmemb, size);
	if (p == NULL) {
		loge("fail to allocate memory");
	}
	return p;
}

static inline void *rezalloc(void *ptr, int nmemb, int size) {
	int origin_size = sizeof(ptr);
	ptr = realloc(ptr, nmemb * size);
	if (ptr == NULL) {
		loge("fail to allocate memory");
	}
	memset(ptr + origin_size, 0, sizeof(ptr) - origin_size);
	return ptr;
}

#endif
