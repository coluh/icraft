#ifndef _ICRAFT_world_world_h
#define _ICRAFT_world_world_h

#include <stdbool.h>
#include "block.h"
#include "chunk.h"
#include "chunktable.h"
#include "generation/generator.h"


typedef struct World {

	Generator *generator;

	ChunkTable chunks;

	struct {
		Chunk **chunks;
		int size;
	} render_list;

	float g;
} World;

World *newWorld();

// return chunk that contains (x, y, z), floored, NULL if not loaded
Chunk *world_chunkAtf(const World *world, float x, float y, float z);
// return chunk that contains (x, y, z), NULL if not loaded
Chunk *world_chunkAti(const World *world, int x, int y, int z);

// update world near given center
void world_update(World *world, int x, int y, int z);
// update nearby chunks
void world_updateChunks(World *world, int x, int y, int z);

// modify block, return original block
BlockID world_modifyBlock(World *w, int x, int y, int z, BlockID block);
// return block at (x, y, z), return air if not loaded
BlockID world_block(const World *w, float x, float y, float z);
// return block at (x, y, z), load if needed
BlockID world_getBlock(World *w, float x, float y, float z);

// from y = 0, rise up and find the first non-air block
int world_topY(World *w, int x, int z);

#endif
