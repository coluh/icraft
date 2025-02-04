#ifndef _ICRAFT_world_chunk_h
#define _ICRAFT_world_chunk_h

#include "block/block.h"
#include <stdbool.h>
#include "../../third_party/cglm/include/cglm/types.h"

#define CHUNK_SIZE	32
#define CHUNK_VOLUME	(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)

typedef struct Chunk {
	int x, y, z;  // the min xyz in this block
	mat4 model;   // model matrix, gened from xyz
	bool generated;
	Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	unsigned VAO, VBO;
	int vertex_count;
	bool dirty;
} Chunk;

void chunk_generateVertex(Chunk *chunk, Chunk *nearbys[6]);

typedef struct ChunkNode {
	Chunk chunk;
	struct ChunkNode *next;
} ChunkNode;

// x, y, z should be devisible by CHUNK_SIZE
// btw return a pointer to the new chunk to you
Chunk *chunks_add(int x, int y, int z);

// returns NULL if list doesn't contain this chunk
Chunk *chunks_find(int x, int y, int z);

void chunks_foreach(void (*handler)(Chunk *chunk));

#endif
