#ifndef _ICRAFT_world_world_h
#define _ICRAFT_world_world_h

#include <stdbool.h>
#include "block.h"
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

World *newWorld();

Chunk *world_chunkAtf(const World *world, float x, float y, float z);
Chunk *world_chunkAti(const World *world, int x, int y, int z);

// update world near given center
void world_update(World *world, int x, int y, int z);
// update nearby chunks
void world_updateChunks(World *world, int x, int y, int z);

BlockID world_modifyBlock(World *w, int x, int y, int z, BlockID block);
BlockID world_block(const World *w, float x, float y, float z);

#endif
