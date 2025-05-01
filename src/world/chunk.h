#ifndef _ICRAFT_world_chunk_h
#define _ICRAFT_world_chunk_h

#include "block/block.h"
#include <stdbool.h>
#include "../../third_party/cglm/include/cglm/types.h"

#define CHUNK_SIZE	32
#define CHUNK_VOLUME	(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)

typedef struct Chunk {
	int x, y, z;  // the min xyz in this block

	Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	mat4 model;   // model matrix, gened from xyz
	unsigned VAO, VBO;
	int vertex_count;

	bool generated;
	bool dirty;
} Chunk;

Chunk *newChunk(int x, int y, int z);

// generate vertex data for the renderer
void chunk_generateVertex(Chunk *chunk, Chunk *nearbys[6]);

// x, y, z should be devisible by CHUNK_SIZE
// btw return a pointer to the new chunk to you


#endif
