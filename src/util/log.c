#include "log.h"
#include "../game.h"
#include <stdio.h>

extern Game g;

void log_init() {
	g.logfile = stdout;
}

void log_setfile(const char *path) {
	FILE *fp = fopen(path, "a");
	g.logfile = fp;
}
