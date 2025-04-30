#ifndef _ICRAFT_world_world_h
#define _ICRAFT_world_world_h

#include <stdbool.h>

// give me a location and I'll load nearby chunks
void world_updateChunks(int x, int y, int z);

bool world_collide(float x, float y, float z, float w, float t, float h);

#endif
