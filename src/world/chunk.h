#ifndef _ICRAFT_world_chunk_h
#define _ICRAFT_world_chunk_h

#include "block/block.h"

#define CHUNK_SIZE	32
#define CHUNK_VOLUME	(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)

#define CHUNK_BLOCK(chunk_ptr, dx, dy, dz) ((chunk_ptr)+(dx)+(dz)*CHUNK_SIZE+(dy)*CHUNK_SIZE*CHUNK_SIZE)

typedef struct Chunk {
	int x, y, z;
	Block blocks[CHUNK_VOLUME];
	unsigned VAO, VBO;
	int vertex_count;
} Chunk;

void chunk_debugFill(Chunk *chunk);

#endif
