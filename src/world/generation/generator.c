#include "generator.h"

void generator_default(Chunk *chunk) {
	for (int dx = 0; dx < CHUNK_SIZE; dx++) {
		for (int dz = 0; dz < CHUNK_SIZE; dz++) {
			int top_height = 20;
			for (int dy = 0; dy < CHUNK_SIZE; dy++) {
				int y = chunk->y + dy;
				int block_id;
				if (y > top_height) { block_id = block_id_of("air"); }
				else if (y > top_height-1) { block_id = block_id_of("grass_block"); }
				else if (y > top_height-4) { block_id = block_id_of("dirt"); }
				else { block_id = block_id_of("stone"); }
				if (y == top_height && (dx == 0 || dz == 0)) { block_id = block_id_of("stone"); }

				block_setId(&chunk->blocks[dx][dy][dz], block_id);
			}
		}
	}
	chunk->dirty = true;
}
