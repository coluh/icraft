#ifndef _ICRAFT_world_chunk_h
#define _ICRAFT_world_chunk_h

#include <stdbool.h>
#include "../../third_party/cglm/include/cglm/types.h"
#include "extralist.h"

#define CHUNK_SIZE	32
#define CHUNK_VOLUME	(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)

typedef struct Chunk {
	int x, y, z;  // the min xyz in this block
	mat4 model;   // model matrix, gened from xyz

	// only stores block id
	int blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	unsigned int VAO, VBO;
	int vertex_count;

	// whether blocks data is generated
	bool generated;
	// whether blocks data is changed and need to modify vertex
	bool dirty;

	BlockExtraList extras;
} Chunk;

Chunk *newChunk(int x, int y, int z);

// generate vertex data for the renderer
void chunk_generateVertex(Chunk *chunk, Chunk *nearbys[6]);

#endif
