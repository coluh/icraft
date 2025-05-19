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
	w->g = -32.0f;
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

BlockID world_block(World *w, float xf, float yf, float zf) {
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

bool world_collide(World *world, const Body *body, float *collide_boundary, int *collide_direction) {
	float x = body->x;
	float y = body->y;
	float z = body->z;
	float w = body->w;
	float h = body->h;
	float d = body->d;
	if (w == 0 && h == 0 && d == 0) {
		return false;
	}
	for (int i = floorf(x); i < ceilf(x + w); i++) {
		for (int j = floorf(y); j < ceilf(y + h); j++) {
			for (int k = floorf(z); k < ceilf(z + d); k++) {
				BlockID block = world_block(world, i, j, k);
				if (block == BLOCK_Air) {
					continue;
				}
				if (((x + w > i) && (x < i + 1.0f)) &&
					((y + h > j) && (y < j + 1.0f)) &&
					((z + d > k) && (z < k + 1.0f))) {
					collide_boundary[0] = x < i ? i : i + 1.0f;
					collide_boundary[1] = y < j ? j : j + 1.0f;
					collide_boundary[2] = z < k ? k : k + 1.0f;
					collide_direction[0] = x < i ? 1 : -1;
					collide_direction[1] = y < j ? 1 : -1;
					collide_direction[2] = z < k ? 1 : -1;
					return true;
				}
			}
		}
	}
	return false;
}

CollisionType world_collisionTest(const Body *stati, const Body *moving, V3 velocity, float *collide_time) {

	const V3 bounding = {
		.x = MAX(stati->x + stati->w, moving->x + moving->w) - MIN(stati->x, moving->x),
		.y = MAX(stati->y + stati->h, moving->y + moving->h) - MIN(stati->y, moving->y),
		.z = MAX(stati->z + stati->d, moving->z + moving->d) - MIN(stati->z, moving->z),
	};
	const float total_time = *collide_time;

	if (velocity.y) {
		const float distance = bounding.y - (stati->h + moving->h);
		*collide_time = distance / ABS(velocity.y);
		const float dt = MAX(*collide_time, 0);
		if (*collide_time < total_time) { // we can hit the static in time
			const float inner_margin = MAX(-0.5f * stati->h, -2.0f); // only if overlap in this range we recover it
			if (((velocity.y > 0 && stati->y - (moving->y + moving->h) > inner_margin) ||
						(velocity.y < 0 && moving->y - (stati->y + stati->h) > inner_margin)) &&
					(MAX(moving->x + moving->w + velocity.x * dt, stati->x + stati->w) - MIN(moving->x + velocity.x * dt, stati->x) < moving->w + stati->w - 0.01) &&
					(MAX(moving->z + moving->d + velocity.z * dt, stati->z + stati->d) - MIN(moving->z + velocity.z * dt, stati->z) < moving->d + stati->d - 0.01)) {
				return Collision_Y;
			}
		} else {
			return Collision_NONE;
		}
	}

	if (velocity.x) {
		const float distance = bounding.x - (stati->w + moving->w);
		*collide_time = distance / ABS(velocity.x);
		const float dt = MAX(*collide_time, 0);
		if (*collide_time < total_time) {
			const float inner_margin = MAX(-0.5f * stati->w, -2.0f);
			if (((velocity.x > 0 && stati->x - (moving->x + moving->w) > inner_margin) ||
						(velocity.x < 0 && moving->x - (stati->x + stati->w) > inner_margin)) &&
					(MAX(moving->y + moving->h + velocity.y * dt, stati->y + stati->h) - MIN(moving->y + velocity.y * dt, stati->y) < moving->h + stati->h - 0.01) &&
					(MAX(moving->z + moving->d + velocity.z * dt, stati->z + stati->d) - MIN(moving->z + velocity.z * dt, stati->z) < moving->d + stati->d - 0.01)) {
				return Collision_X;
			}
		} else {
			return Collision_NONE;
		}
	}

	if (velocity.z) {
		const float distance = bounding.z - (stati->d + moving->d);
		*collide_time = distance / ABS(velocity.z);
		const float dt = MAX(*collide_time, 0);
		if (*collide_time < total_time) {
			const float inner_margin = MAX(-0.5f * stati->d, -2.0f);
			if (((velocity.z > 0 && stati->z - (moving->z + moving->d) > inner_margin) ||
						(velocity.z < 0 && moving->z - (stati->z + stati->d) > inner_margin)) &&
					(MAX(moving->x + moving->w + velocity.x * dt, stati->x + stati->w) - MIN(moving->x + velocity.x * dt, stati->x) < moving->w + stati->w - 0.01) &&
					(MAX(moving->y + moving->h + velocity.y * dt, stati->y + stati->h) - MIN(moving->y + velocity.y * dt, stati->y) < moving->h + stati->h - 0.01)) {
				return Collision_Z;
			}
		} else {
			return Collision_NONE;
		}
	}

	return Collision_NONE;
}
