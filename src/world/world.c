#include "world.h"
#include <math.h>
#include <stdbool.h>
#include "block/block.h"
#include "block/extra.h"
#include "chunk.h"
#include "../util/mem.h"
#include "../util/props.h"
#include "extralist.h"
#include "generation/generator.h"
#include <stddef.h>

extern Game g;

World *newWorld() {
	World *w = zalloc(1, sizeof(World));
	w->g = -32.0f;
	return w;
}

// find the chunk in x, y, z
static Chunk *findChunk(const World *w, int x, int y, int z) {
	for (ChunkNode *p = w->chunks; p != NULL; p = p->next) {
		if ((p->chunk->x == x) && (p->chunk->y == y) && (p->chunk->z == z)) {
			return p->chunk;
		}
	}
	return NULL;
}

Chunk *world_chunkAtf(World *world, float x, float y, float z) {
	int cx = ROUND_DOWN_BY((int)floorf(x), CHUNK_SIZE);
	int cy = ROUND_DOWN_BY((int)floorf(y), CHUNK_SIZE);
	int cz = ROUND_DOWN_BY((int)floorf(z), CHUNK_SIZE);
	return findChunk(world, cx, cy, cz);
}

Chunk *world_chunkAti(World *world, int x, int y, int z) {
	int cx = ROUND_DOWN_BY(x, CHUNK_SIZE);
	int cy = ROUND_DOWN_BY(y, CHUNK_SIZE);
	int cz = ROUND_DOWN_BY(z, CHUNK_SIZE);
	return findChunk(world, cx, cy, cz);
}

// use for convenience
// same order as in src/world/chunk.c
static const int chunk_offsets[6][3] = {
	{0, 0, -CHUNK_SIZE}, {0, 0, CHUNK_SIZE},
	{0, -CHUNK_SIZE, 0}, {0, CHUNK_SIZE, 0},
	{-CHUNK_SIZE, 0, 0}, {CHUNK_SIZE, 0, 0},
};

// load the chunk at x, y, z
static Chunk *loadChunk(World *w, int x, int y, int z) {
	Chunk *newly = newChunk(x, y, z);
	generator_default(newly);
	chunk_generateVertex(newly, (Chunk*[]){NULL, NULL, NULL, NULL, NULL, NULL});

	// update nearby vertex, reducing some faces
	for (int f = 0; f < 6; f++) {
		Chunk *nearby = findChunk(w, x+chunk_offsets[f][0],
				y+chunk_offsets[f][1], z+chunk_offsets[f][2]);
		if (nearby != NULL) {
			nearby->dirty = true;
		}
	}

	// add to world chunk list
	ChunkNode *n = zalloc(1, sizeof(ChunkNode));
	n->chunk = newly;
	ChunkNode **p = &w->chunks;
	while (*p != NULL) {
		p = &(*p)->next;
	}
	*p = n;
	return newly;
}

// update chunk blocks
// update blockextra states;
// update vertex data;
static void world_updateChunk(World *w, Chunk *c) {

	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				if (block_isPlant(c->blocks[x][y][z])) {
					// TODO: cross chunk update
					if (y > 0 && !block_isOpaqueBlock(c->blocks[x][y-1][z])) {
						block_destroy(w, c->x+x, c->y+y, c->z+z);
						c->dirty = true;
					}
				}
			}
		}
	}

	extralist_update(&c->extras);

	// update vertex data
	if (c->dirty) {
		Chunk *nearbys[6];
		for (int f = 0; f < 6; f++) {
			nearbys[f] = findChunk(w, c->x+chunk_offsets[f][0],
					c->y+chunk_offsets[f][1], c->z+chunk_offsets[f][2]);
		}
		chunk_generateVertex(c, nearbys);
		c->dirty = false;
	}

}

void world_updateChunks(World *w, int x, int y, int z) {
	x = ROUND_DOWN_BY(x, CHUNK_SIZE);
	y = ROUND_DOWN_BY(y, CHUNK_SIZE);
	z = ROUND_DOWN_BY(z, CHUNK_SIZE);

	// maybe activate like 5 chunks:
	//   [][][]
	// [][][][][]
	// [][][][][]
	// [][][][][]
	//   [][][]

	for (int i = -1; i <= 1; i++) {
		for (int j = -2; j <= 2; j += 4) {
			for (int k = -1; k <= 1; k++) {
				int cx = x + i * CHUNK_SIZE;
				int cy = y + j * CHUNK_SIZE;
				int cz = z + k * CHUNK_SIZE;
				if (!findChunk(w, cx, cy, cz)) {
					loadChunk(w, cx, cy, cz);
				}
			}
		}
	}
	for (int i = -2; i <= 2; i++) {
		for (int j = -1; j <= 1; j++) {
			for (int k = -2; k <= 2; k++) {
				int cx = x + i * CHUNK_SIZE;
				int cy = y + j * CHUNK_SIZE;
				int cz = z + k * CHUNK_SIZE;
				if (!findChunk(w, cx, cy, cz)) {
					loadChunk(w, cx, cy, cz);
				}
			}
		}
	}

	// only update these chunks
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			for (int k = -1; k <= 1; k++) {
				int cx = x + i * CHUNK_SIZE;
				int cy = y + j * CHUNK_SIZE;
				int cz = z + k * CHUNK_SIZE;
				world_updateChunk(w, findChunk(w, cx, cy, cz));
			}
		}
	}
}

BlockID world_modifyBlock(World *w, int x, int y, int z, BlockID block) {
	Chunk *c = world_chunkAti(w, x, y, z);
	int dx = x - c->x;
	int dy = y - c->y;
	int dz = z - c->z;
	BlockID ret = c->blocks[dx][dy][dz];
	c->blocks[dx][dy][dz] = block;
	c->dirty = true;

	Chunk *n = NULL;
	if ((dx == 0) && ((n = findChunk(w, c->x-CHUNK_SIZE, c->y, c->z)))) {
		n->dirty = true;
	} else if ((dx == CHUNK_SIZE-1) && ((n = findChunk(w, c->x+CHUNK_SIZE, c->y, c->z)))) {
		n->dirty = true;
	}
	if ((dy == 0) && ((n = findChunk(w, c->x, c->y-CHUNK_SIZE, c->z)))) {
		n->dirty = true;
	} else if ((dy == CHUNK_SIZE-1) && ((n = findChunk(w, c->x, c->y+CHUNK_SIZE, c->z)))) {
		n->dirty = true;
	}
	if ((dz == 0) && ((n = findChunk(w, c->x, c->y, c->z-CHUNK_SIZE)))) {
		n->dirty = true;
	} else if ((dz == CHUNK_SIZE-1) && ((n = findChunk(w, c->x, c->y, c->z+CHUNK_SIZE)))) {
		n->dirty = true;
	}

	return ret;
}

BlockID world_block(const World *w, float xf, float yf, float zf) {
	int x = floorf(xf);
	int y = floorf(yf);
	int z = floorf(zf);
	int cx = ROUND_DOWN_BY(x, CHUNK_SIZE);
	int cy = ROUND_DOWN_BY(y, CHUNK_SIZE);
	int cz = ROUND_DOWN_BY(z, CHUNK_SIZE);
	Chunk *chunk = findChunk(w, cx, cy, cz);
	if (chunk == NULL) {
		// debug
		return BLOCK_Air; // air
	}
	return chunk->blocks[x-cx][y-cy][z-cz];
}

