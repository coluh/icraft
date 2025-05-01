#include "world.h"
#include <math.h>
#include <stdbool.h>
#include "block/block.h"
#include "chunk.h"
#include "../util/mem.h"
#include "../util/props.h"
#include "generation/generator.h"
#include <stddef.h>

World *newWorld() {
	World *w = zalloc(1, sizeof(World));
	w->g = -9.8;
	return w;
}

// find the chunk in x, y, z
static Chunk *findChunk(World *w, int x, int y, int z) {
	for (ChunkNode *p = w->chunks; p != NULL; p = p->next) {
		if ((p->chunk->x == x) && (p->chunk->y == y) && (p->chunk->z == z)) {
			return p->chunk;
		}
	}
	return NULL;
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
	if (!newly->generated) {
		generator_default(newly);

		// update nearby vertex, reducing some faces
		for (int f = 0; f < 6; f++) {
			Chunk *nearby = findChunk(w, x+chunk_offsets[f][0],
					y+chunk_offsets[f][1], z+chunk_offsets[f][2]);
			if (nearby != NULL) {
				nearby->dirty = true;
			}
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

// update the vertex data
static void world_updateChunk(World *w, Chunk *chunk) {
	if (chunk->dirty) {
		Chunk *nearbys[6];
		for (int f = 0; f < 6; f++) {
			nearbys[f] = findChunk(w, chunk->x+chunk_offsets[f][0],
					chunk->y+chunk_offsets[f][1], chunk->z+chunk_offsets[f][2]);
		}
		chunk_generateVertex(chunk, nearbys);
		chunk->dirty = false;

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

	/* link 81 chunks */
	// 27 chunks now in debug
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			for (int k = -1; k <= 1; k++) {
				int cx = x + i * CHUNK_SIZE;
				int cy = y + j * CHUNK_SIZE;
				int cz = z + k * CHUNK_SIZE;
				Chunk *c = findChunk(w, cx, cy, cz);
				if (c == NULL) {
					c = loadChunk(w, cx, cy, cz);
				}
				world_updateChunk(w, c);
			}
		}
	}
}

Block world_block(World *w, int x, int y, int z) {
	int cx = ROUND_DOWN_BY(x, CHUNK_SIZE);
	int cy = ROUND_DOWN_BY(y, CHUNK_SIZE);
	int cz = ROUND_DOWN_BY(z, CHUNK_SIZE);
	Chunk *chunk = findChunk(w, cx, cy, cz);
	if (chunk == NULL) {
		// debug
		return 0; // air
	}
	return chunk->blocks[x-cx][y-cy][z-cz];
}

bool world_collide(World *world, Body *body) {
	float x = body->x;
	float y = body->y;
	float z = body->z;
	float w = body->w;
	float h = body->h;
	float t = body->t;
	for (int i = floorf(x); i < ceilf(x + w); i++) {
		for (int j = floorf(y); j < ceilf(y + h); j++) {
			for (int k = floorf(z); k < ceilf(z + t); k++) {
				Block block = world_block(world, i, j, k);
				if (block_getId(block) == block_id_of("air")) {
					continue;
				}
				if (((x + w > i) && (x < i + 1)) &&
					((y + h > j) && (y < j + 1)) &&
					((z + t > k) && (z < k + 1))) {
					return true;
				}
			}
		}
	}
	return false;
}
