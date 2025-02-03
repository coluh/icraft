#ifndef _ICRAFT_util_log_h
#define _ICRAFT_util_log_h

#include "./time.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE *logfile;

#define LOG(level, ...) do { \
	fprintf(logfile, "[%s] [%s] %s: ", time_get(), level, __FUNCTION__); \
	fprintf(logfile, __VA_ARGS__); \
	fprintf(logfile, "\n"); \
	if (logfile == stdout) fflush(stdout); \
} while (0)

#define logv(...) LOG("VERBOSE", __VA_ARGS__)
#define logd(...) LOG("DEBUG", __VA_ARGS__)

#define logi(...) LOG("INFO", __VA_ARGS__)
#define logw(...) LOG("WARNING", __VA_ARGS__)
#define loge(...) do { \
	LOG("ERROR", __VA_ARGS__); \
	exit(EXIT_FAILURE); \
} while (0)

#define Assert(b, ...) do { \
	if (!(b)) { loge(__VA_ARGS__); } \
} while (0)

void log_init();
void log_setfile(const char *path);

#endif
