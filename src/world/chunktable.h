#ifndef _ICRAFT_world_chunktable_h
#define _ICRAFT_world_chunktable_h

#include "chunk.h"
#include "generation/generator.h"

#define CHUNK_HASH_SIZE 4096

typedef struct ChunkNode {
	Chunk *chunk;
	struct ChunkNode *next_in_bucket;
} ChunkNode;

typedef struct {
	ChunkNode *buckets[CHUNK_HASH_SIZE];
} ChunkTable;

// get the chunk in x, y, z, or NULL if not exist
// coordinates should be aligned to CHUNK_SIZE
Chunk *chunk_get(const ChunkTable *table, int x, int y, int z);

// get the chunk in x, y, z, load if not found
// coordinates should be aligned to CHUNK_SIZE
Chunk *chunk_getLoad(ChunkTable *table, int x, int y, int z, Generator *gen, bool async);

#endif
