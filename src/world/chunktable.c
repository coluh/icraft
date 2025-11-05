#include "chunktable.h"
#include <stdint.h>
#include "../util/mem.h"
#include "chunk.h"
#include "generation/generator.h"
#include <SDL2/SDL_thread.h>

// hash chunk.x .y .z
static unsigned int chunk_hash(int x, int y, int z) {
	x /= CHUNK_SIZE;
	y /= CHUNK_SIZE;
	z /= CHUNK_SIZE;
	uint32_t h = (uint32_t)(x * 73856093) ^ (y * 19349663) ^ (z * 83492791);
	return h % CHUNK_HASH_SIZE;
}

// insert chunk at x, y, z
// coordinates should be aligned to CHUNK_SIZE
static void chunk_insert(ChunkNode *buckets[], Chunk *chunk) {
	ChunkNode *n = zalloc(1, sizeof(ChunkNode));
	n->chunk = chunk;

	unsigned int h = chunk_hash(chunk->x, chunk->y, chunk->z);
	n->next_in_bucket = buckets[h];
	buckets[h] = n;
}

Chunk *chunk_get(const ChunkTable *table, int x, int y, int z) {
	unsigned int h = chunk_hash(x, y, z);
	for (const ChunkNode *p = table->buckets[h]; p != NULL; p = p->next_in_bucket) {
		if (p->chunk->x == x && p->chunk->y == y && p->chunk->z == z) {
			return p->chunk;
		}
	}
	return NULL;
}

struct chunkgen_args {
	Generator *generator;
	Chunk *chunk;
};

static int chunkgen(void *argv) {
	struct chunkgen_args args = *(struct chunkgen_args *)argv;
	Generator *generator = args.generator;
	Chunk *chunk = args.chunk;
	// generate blocks
	generator_fill(generator, chunk);

	free(argv);
	return 0;
}

Chunk *chunk_getLoad(ChunkTable *table, int x, int y, int z, Generator *gen, bool async) {
	Chunk *c = chunk_get(table, x, y, z);
	if (c != NULL) {
		return c;
	}

	c = newChunk(x, y, z);
	// async = false;
	if (!async) {
		generator_fill(gen, c);
	} else {
		struct chunkgen_args *argv = zalloc(1, sizeof(struct chunkgen_args));
		argv->generator = gen;
		argv->chunk = c;
		SDL_Thread *thread = SDL_CreateThread(chunkgen, "ChunkGen", argv);
		SDL_DetachThread(thread);
	}

	chunk_insert(table->buckets, c);

	return c;
}
