#ifndef _ICRAFT_world_world_h
#define _ICRAFT_world_world_h

#include <stdbool.h>
#include "chunk.h"

typedef struct ChunkNode {
	Chunk *chunk;
	struct ChunkNode *next;
} ChunkNode;

typedef struct World {
	// chunk list
	ChunkNode *chunks;

	float g;
} World;

typedef struct Body {
	float x, y, z;
	float w, h, t;
} Body;

#define BODY(x, y, z, w, h, t) (&(Body){x,y,z,w,h,t})

World *newWorld();

// update nearby chunks
void world_updateChunks(World *world, int x, int y, int z);

bool world_collide(World *world, Body *body);

#endif
