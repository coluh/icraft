#include "./time.h"

#include <time.h>
#include <stdio.h>

const char *time_get() {
	static char buffer[20];

	time_t seconds = time(NULL);
	struct tm *l = localtime(&seconds);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", l);

	return buffer;
}
