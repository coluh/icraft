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
	memset((char*)ptr + origin_size, 0, sizeof(ptr) - origin_size);
	return ptr;
}

static inline void *dupalloc(void *ptr) {
	int size = sizeof(ptr);
	void *p = malloc(size);
	memcpy(p, ptr, size);
	return p;
}

static inline void *dupnalloc(void *ptr, int size) {
	void *p = malloc(size);
	memcpy(p, ptr, size);
	return p;
}

#endif
