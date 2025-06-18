#include <stdlib.h>

void props_init() {
	srand(42);
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

int count_on_layer(int d) {
	if (d < 0) {
		return 0;
	} else if (d == 0) {
		return 1;
	} else {
		return 4*d*d + 2;
	}
}

void np_z(int n, int *xp, int *yp, int *zp) {
	if (n <= 0) {
		*xp = *yp = *zp = 0;
		return;
	}

	int d = 1, total = 0;
	while (1) {
		int count = count_on_layer(d);
		if (n <= total + count) {
			break;
		}
		total += count;
		d++;
	}

	int idx = n - total - 1;

	// point idx on layer d;
	int count = 0;
	for (int x = -d; x <= d; x++) {
		for (int y = -d; y <= d; y++) {
			int abs_z = d - abs(x) - abs(y);
			if (abs_z < 0) { continue; }

			for (int z = -abs_z; z <= abs_z; z += 2*abs_z) {
				if (abs(x) + abs(y) + abs_z != d) {
					continue;
				}
				if (count == idx) {
					*xp = x;
					*yp = y;
					*zp = z;
					return;
				}
				count++;
				if (abs_z == 0) {
					break;
				}
			}
		}
	}
}

