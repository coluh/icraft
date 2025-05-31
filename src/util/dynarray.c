#include "dynarray.h"
#include <string.h>
#include "mem.h"

DynArray *dynarray_create(int elem_size) {
	DynArray *da = zalloc(1, sizeof(DynArray));
	da->elem_size = elem_size;
	da->capacity = 8;
	da->data = zalloc(da->capacity, elem_size);
	return da;
}

void dynarray_push(DynArray *da, void *v) {
	void *p = (char*)da->data + da->size*da->elem_size;
	memcpy(p, v, da->elem_size);
	da->size++;

	if (da->size == da->capacity) {
		da->capacity *= 2;
		da->data = rezalloc(da->data, da->capacity, da->elem_size);
	}
}

void dynarray_free(DynArray *da) {
	free(da->data);
	free(da);
}
