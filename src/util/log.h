#ifndef _ICRAFT_util_log_h
#define _ICRAFT_util_log_h

#include "./date.h"
#include <stdlib.h>
#include "../game.h"

extern Game g;

#define LOG(level, ...) do { \
	fprintf(g.logfile, "[%s] [%s] %s: ", time_get(), level, __FUNCTION__); \
	fprintf(g.logfile, __VA_ARGS__); \
	fprintf(g.logfile, "\n"); \
	if (g.logfile == stdout) fflush(stdout); \
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
