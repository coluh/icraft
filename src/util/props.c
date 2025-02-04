#include <stdlib.h>
#include <time.h>

void props_init() {
	srand(time(0));
}

int rand_int(int min, int max) {
	return min + rand() % (max-min);
}
