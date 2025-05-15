#include <stdlib.h>
#include <time.h>

void props_init() {
	srand(time(0));
}

int rand_int(int min, int max) {
	return min + rand() % (max-min);
}

float rand_float(float min, float max) {
	float r = (float)rand() / RAND_MAX;
	return min + r * (max-min);
}

int max_field_int_p(void *strp_arr[], int field_offset, int count) {
	int max = *(int*)((char*)(strp_arr[0]) + field_offset);
	for (int i = 0; i < count; i++) {
		int value = *(int*)((char*)(strp_arr[i]) + field_offset);
		if (max < value) max = value;
	}
	return max;
}

int sum_field_int_p(void *strp_arr[], int field_offset, int count) {
	int sum = 0;
	for (int i = 0; i < count; i++) {
		sum += *(int*)((char*)(strp_arr[i]) + field_offset);
	}
	return sum;
}
