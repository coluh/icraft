#include "world.h"
#include <math.h>
#include <stdbool.h>
#include "block.h"
#include "blockstate/blockstatelist.h"
#include "../entity/entity.h"
#include "chunk.h"
#include "../util/mem.h"
#include "../util/props.h"
#include "chunktable.h"
#include "generation/generator.h"
#include <stddef.h>
#include <SDL2/SDL_thread.h>

// TODO: different kinds of getChunk, load, generated, wait...

extern Game g;

// this is debug implementaion
#define LOAD_DISTANCE 6

World *newWorld() {
	World *w = zalloc(1, sizeof(World));
	w->generator = generator_create(42); // TODO: seed
					     //
	// w->render_list.size = 27+9*6;
	w->render_list.size = LOAD_DISTANCE*LOAD_DISTANCE*LOAD_DISTANCE*8;
	w->render_list.chunks = zalloc(w->render_list.size, sizeof(Chunk*));
	w->g = -32.0f;

	generator_generate(w->generator);

	return w;
}

Chunk *world_chunkAti(const World *world, int x, int y, int z) {
	int cx = ROUND_DOWN_BY(x, CHUNK_SIZE);
	int cy = ROUND_DOWN_BY(y, CHUNK_SIZE);
	int cz = ROUND_DOWN_BY(z, CHUNK_SIZE);
	return chunk_get(&world->chunks, cx, cy, cz);
}

Chunk *world_chunkAtf(const World *world, float x, float y, float z) {
	return world_chunkAti(world, floorf(x), floorf(y), floorf(z));
}

// update chunk blocks
// update blockstates
// update vertex data
static void world_updateChunk(World *w, Chunk *c) {

	if (!c->generated) {
		return;
	}

	// blocks
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				if (BLOCK_ISPLANT(c->blocks[x][y][z])) {
					// TODO: cross chunk update
					if (y > 0 && BLOCK_ISWATERABLE(c->blocks[x][y-1][z])) {
						BlockID origin = world_modifyBlock(w, c->x+x, c->y+y, c->z+z, BLOCK_Air);
						Entity *drops = entity_get(g.entities, entity_create(Entity_DROPS, (V3){c->x+x+0.5f, c->y+y+0.5f, c->z+z+0.5f}, g.entities));
						drops->drops.item.id = block_get(origin)->break_item;
						drops->velocity.x = rand_float(-2.0f, 2.0f);
						drops->velocity.y = rand_float(2.0f, 5.0f);
						drops->velocity.z = rand_float(-2.0f, 2.0f);
						c->dirty = true;
					}
				}
			}
		}
	}

	// blockstates
	blockstatelist_update(&c->block_states, c, w);

	// use for convenience
	// same order as in src/world/chunk.c
	static const int chunk_offsets[6][3] = {
		{0, 0, -CHUNK_SIZE}, {0, 0, CHUNK_SIZE},
		{0, -CHUNK_SIZE, 0}, {0, CHUNK_SIZE, 0},
		{-CHUNK_SIZE, 0, 0}, {CHUNK_SIZE, 0, 0},
	};
	// update vertex data
	if (c->dirty) {
		const Chunk *nearbys[6];
		for (int f = 0; f < 6; f++) {
			nearbys[f] = chunk_get(&w->chunks, c->x+chunk_offsets[f][0],
					c->y+chunk_offsets[f][1], c->z+chunk_offsets[f][2]);
		}
		chunk_generateVertex(c, nearbys);
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

	int i_render_chunks = 0;
	for (int i = -LOAD_DISTANCE; i < LOAD_DISTANCE; i++) {
		for (int j = -LOAD_DISTANCE; j < LOAD_DISTANCE; j++) {
			for (int k = -LOAD_DISTANCE; k < LOAD_DISTANCE; k++) {
				int cx = x + i * CHUNK_SIZE;
				int cy = y + j * CHUNK_SIZE;
				int cz = z + k * CHUNK_SIZE;
				Chunk *c = chunk_getLoad(&w->chunks, cx, cy, cz, w->generator, true);
				w->render_list.chunks[i_render_chunks++] = c;
			}
		}
	}

	// this is for chunks that is generated but have no chance to build vertex
	int build_count = 4;
	while (build_count > 0) {
		for (int i = 0; i < w->render_list.size; i++) {
			Chunk *c = w->render_list.chunks[i];
			if (c->generated && c->dirty) {
				chunk_generateVertex(c, (const Chunk *[]){NULL, NULL, NULL, NULL, NULL, NULL});
				break;
			}
		}
		build_count--;
	}

	// only update these chunks
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			for (int k = -1; k <= 1; k++) {
				int cx = x + i * CHUNK_SIZE;
				int cy = y + j * CHUNK_SIZE;
				int cz = z + k * CHUNK_SIZE;
				world_updateChunk(w, chunk_getLoad(&w->chunks, cx, cy, cz, w->generator, false));
			}
		}
	}
}

void world_update(World *w, int x, int y, int z) {
	world_updateChunks(w, x, y, z);
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
	if ((dx == 0) && ((n = chunk_get(&w->chunks, c->x-CHUNK_SIZE, c->y, c->z)))) {
		n->dirty = true;
	} else if ((dx == CHUNK_SIZE-1) && ((n = chunk_get(&w->chunks, c->x+CHUNK_SIZE, c->y, c->z)))) {
		n->dirty = true;
	}
	if ((dy == 0) && ((n = chunk_get(&w->chunks, c->x, c->y-CHUNK_SIZE, c->z)))) {
		n->dirty = true;
	} else if ((dy == CHUNK_SIZE-1) && ((n = chunk_get(&w->chunks, c->x, c->y+CHUNK_SIZE, c->z)))) {
		n->dirty = true;
	}
	if ((dz == 0) && ((n = chunk_get(&w->chunks, c->x, c->y, c->z-CHUNK_SIZE)))) {
		n->dirty = true;
	} else if ((dz == CHUNK_SIZE-1) && ((n = chunk_get(&w->chunks, c->x, c->y, c->z+CHUNK_SIZE)))) {
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
	Chunk *chunk = chunk_get(&w->chunks, cx, cy, cz);
	if (chunk == NULL || !chunk->generated) {
		return BLOCK_Air;
	}
	return chunk->blocks[x-cx][y-cy][z-cz];
}

BlockID world_getBlock(World *w, float xf, float yf, float zf) {
	int x = floorf(xf);
	int y = floorf(yf);
	int z = floorf(zf);
	int cx = ROUND_DOWN_BY(x, CHUNK_SIZE);
	int cy = ROUND_DOWN_BY(y, CHUNK_SIZE);
	int cz = ROUND_DOWN_BY(z, CHUNK_SIZE);
	Chunk *chunk = chunk_getLoad(&w->chunks, cx, cy, cz, w->generator, false);
	return chunk->blocks[x-cx][y-cy][z-cz];
}

int world_topY(World *w, int x, int z) {
	int y = 0;
	while (world_getBlock(w, x, y, z) != BLOCK_Air) {
		y++;
	}
	return y;
}
