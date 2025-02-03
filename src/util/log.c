#include "log.h"
#include <stdio.h>

FILE *logfile;

void log_init() {
	logfile = stdout;
}

void log_setfile(const char *path) {
	FILE *fp = fopen(path, "a");
	logfile = fp;
}
