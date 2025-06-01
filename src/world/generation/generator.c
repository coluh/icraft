#include "generator.h"
#include "../block.h"
#include "../../util/props.h"

void generator_default(Chunk *chunk) {
	for (int dx = 0; dx < CHUNK_SIZE; dx++) {
		for (int dz = 0; dz < CHUNK_SIZE; dz++) {
			// int top= rand_int(18, 21);
			int top= 20;
			for (int dy = 0; dy < CHUNK_SIZE; dy++) {
				int y = chunk->y + dy;
				BlockID id = y > top ? BLOCK_Air : (y > top-1 ? BLOCK_GrassBlock : (y > top-4 ? BLOCK_Dirt : BLOCK_Stone));
				if (y == top && (dx == 0 || dz == 0)) {
					id = BLOCK_Stone;
				}
				if (y == top + 1 && rand_float(0, 1) < 0.1f) {
					id = rand_float(0, 1) < 0.5f ? BLOCK_Poppy : BLOCK_Dandelion;
				}

				if (id != BLOCK_Air) {
					chunk->blocks[dx][dy][dz] = id;
				}
			}
			if (rand_float(0, 1) < 0.01f) {
				int height = rand_int(4, 7);
				int cy = top + height;
				if (cy >= CHUNK_SIZE - 2 || dx < 5 || dx > CHUNK_SIZE - 5 || dz < 5 || dz > CHUNK_SIZE - 5) {
					continue;
				}
				if (cy + chunk->y > 40 || cy + chunk->y < 0) {
					continue;
				}
				for (int dy = top + 1; dy <= cy; dy++) {
					chunk->blocks[dx][dy][dz] = BLOCK_OakLog;
				}
				chunk->blocks[dx+1][cy][dz] = BLOCK_OakLeave;
				chunk->blocks[dx][cy][dz+1] = BLOCK_OakLeave;
				chunk->blocks[dx-1][cy][dz] = BLOCK_OakLeave;
				chunk->blocks[dx][cy][dz-1] = BLOCK_OakLeave;
				chunk->blocks[dx][cy+1][dz] = BLOCK_OakLeave;
			}
		}
	}
	if (chunk->x == 0 && chunk->y == 0 && chunk->z == 0) {
		chunk->blocks[0][21][0] = BLOCK_Unknown;
	}
	chunk->generated = true;
	chunk->dirty = true;
}
