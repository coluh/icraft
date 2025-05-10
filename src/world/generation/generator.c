#include "generator.h"
#include "../block/block.h"
#include "../../util/props.h"

void generator_default(Chunk *chunk) {
	for (int dx = 0; dx < CHUNK_SIZE; dx++) {
		for (int dz = 0; dz < CHUNK_SIZE; dz++) {
			int top= rand_int(18, 21);
			for (int dy = 0; dy < CHUNK_SIZE; dy++) {
				int y = chunk->y + dy;
				BlockID id = y > top ? BLOCK_Air : (y > top-1 ? BLOCK_GrassBlock : (y > top-4 ? BLOCK_Dirt : BLOCK_Stone));
				if (y == top && (dx == 0 || dz == 0)) {
					id = BLOCK_Stone;
				}

				chunk->blocks[dx][dy][dz] = id;
			}
		}
	}
	chunk->generated = true;
	chunk->dirty = true;
}
