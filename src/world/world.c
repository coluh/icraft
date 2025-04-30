#include "world.h"
#include <math.h>
#include <stdbool.h>
#include "block/block.h"
#include "chunk.h"
#include "../util/log.h"
#include "../util/props.h"
#include "generation/generator.h"
#include <stddef.h>

// same order as in src/world/chunk.c
static const int chunk_offsets[6][3] = {
	{0, 0, -CHUNK_SIZE}, {0, 0, CHUNK_SIZE},
	{0, -CHUNK_SIZE, 0}, {0, CHUNK_SIZE, 0},
	{-CHUNK_SIZE, 0, 0}, {CHUNK_SIZE, 0, 0},
};

static void world_loadChunk(int x, int y, int z) {
	Chunk *newly = chunks_add(x, y, z);
	if (!newly->generated) {
		generator_default(newly);

		for (int f = 0; f < 6; f++) {
			Chunk *nearby = chunks_find(
					x+chunk_offsets[f][0], y+chunk_offsets[f][1], z+chunk_offsets[f][2]);
			if (nearby != NULL) {
				nearby->dirty = true;
			}
		}
	}
}

static void world_updateChunk(Chunk *chunk) {
	if (!chunk->dirty) {
		return;
	}

	Chunk *nearbys[6];
	for (int f = 0; f < 6; f++) {
		nearbys[f] = chunks_find(
				chunk->x+chunk_offsets[f][0], chunk->y+chunk_offsets[f][1], chunk->z+chunk_offsets[f][2]);
	}
	chunk_generateVertex(chunk, nearbys);
	chunk->dirty = false;
}

void world_updateChunks(int x, int y, int z) {
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
	// nono 27 chunks now in debug
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			for (int k = -1; k <= 1; k++) {
				int cx = x + i * CHUNK_SIZE;
				int cy = y + j * CHUNK_SIZE;
				int cz = z + k * CHUNK_SIZE;
				if (!chunks_find(cx, cy, cz)) {
					world_loadChunk(cx, cy, cz);
				}
			}
		}
	}

	chunks_foreach(world_updateChunk);
}

Block world_block(int x, int y, int z) {
	int cx = ROUND_DOWN_BY(x, CHUNK_SIZE);
	int cy = ROUND_DOWN_BY(y, CHUNK_SIZE);
	int cz = ROUND_DOWN_BY(z, CHUNK_SIZE);
	Chunk *chunk = chunks_find(cx, cy, cz);
	if (chunk == NULL) {
		// debug
		return 0; // air
	}
	return chunk->blocks[x-cx][y-cy][z-cz];
}

bool world_collide(float x, float y, float z, float w, float t, float h) {
	for (int i = floorf(x); i < ceilf(x + w); i++) {
		for (int j = floorf(y); j < ceilf(y + t); j++) {
			for (int k = floorf(z); k < ceilf(z + h); k++) {
				Block block = world_block(i, j, k);
				if (block_getId(block) == block_id_of("air")) {
					continue;
				}
				if (((x + w > i) && (x < i + 1)) &&
					((y + t > j) && (y < j + 1)) &&
					((z + h > k) && (z < k + 1))) {
					return true;
				}
			}
		}
	}
	return false;
}
