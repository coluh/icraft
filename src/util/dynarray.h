#ifndef _ICRAFT_util_dynarray_h
#define _ICRAFT_util_dynarray_h

typedef struct DynArray {
	void *data;
	int size;
	int capacity;
	int elem_size;
} DynArray;

DynArray *dynarray_create(int elem_size);
void dynarray_push(DynArray *da, void *v);
void dynarray_free(DynArray *da);

#endif
