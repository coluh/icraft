#include "generator.h"

void generator_default(Chunk *chunk) {
	for (int dx = 0; dx < CHUNK_SIZE; dx++) {
		for (int dz = 0; dz < CHUNK_SIZE; dz++) {
			for (int dy = 0; dy < CHUNK_SIZE; dy++) {
				int y = chunk->y + dy;
				int block_id = block_id_of("air");
				if (y < 32) { block_id = block_id_of("grass_block"); }
				if (y < 31) { block_id = block_id_of("dirt"); }
				if (y < 28) { block_id = block_id_of("stone"); }
				block_setId(&chunk->blocks[dx][dy][dz], block_id);
			}
		}
	}
	chunk->dirty = true;
}
