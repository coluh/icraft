#include "chunk.h"
#include <glad/glad.h>
#include <string.h>
#include "../../third_party/cglm/include/cglm/types.h"
#include "block/block.h"

#define CHUNK_MAX_VERTICE_COUNT (CHUNK_VOLUME*6*6)

struct vertex {
	vec3 position;
	vec3 normal;
	vec2 uv;
} vertices[CHUNK_MAX_VERTICE_COUNT];

// -z, z, -y, y, -x, x
static void generate_face(int f, struct vertex face[], int x, int y, int z, int id) {
	static int indices[6][6][3] = {
		{
			{0, 0, 0}, {1, 0, 0}, {1, 1, 0},
			{0, 0, 0}, {1, 1, 0}, {0, 1, 0},
		},
		{
			{0, 0, 1}, {0, 1, 1}, {1, 1, 1},
			{0, 0, 1}, {1, 1, 1}, {1, 0, 1},
		},
		{
			{0, 0, 0}, {1, 0, 0}, {1, 0, 1},
			{0, 0, 0}, {1, 0, 1}, {0, 0, 1},
		},
		{
			{0, 1, 0}, {0, 1, 1}, {1, 1, 1},
			{0, 1, 0}, {1, 1, 1}, {1, 1, 0},
		},
		{
			{0, 0, 0}, {0, 1, 0}, {0, 1, 1},
			{0, 0, 0}, {0, 1, 1}, {0, 0, 1},
		},
		{
			{1, 0, 0}, {1, 0, 1}, {1, 1, 1},
			{1, 0, 0}, {1, 1, 1}, {1, 1, 0},
		},
	};
	static int norms[6][3] = {
		{0, 0, -1}, {0, 0, 1},
		{0, -1, 0}, {0, 1, 0},
		{-1, 0, 0}, {1, 0, 0},
	};  // xyz, xzy, yxz, yzx, zxy, zyx
	static int uvs[6][2] = {
		{0, 0}, {1, 0}, {1, 1},
		{0, 0}, {1, 1}, {0, 1},
	};

	// tow triangles
	for (int i = 0; i < 6; i++) {
		face[i].position[0] = x + indices[f][i][0];
		face[i].position[1] = y + indices[f][i][1];
		face[i].position[2] = z + indices[f][i][2];
		face[i].normal[0] = norms[f][0];
		face[i].normal[1] = norms[f][1];
		face[i].normal[2] = norms[f][2];
		face[i].uv[0] = (float)(uvs[i][0] + id % BLOCK_TEXTURE_ROW_COUNT) / BLOCK_TEXTURE_ROW_COUNT;
		face[i].uv[1] = (float)(uvs[i][1] + (int)(id / BLOCK_TEXTURE_ROW_COUNT)) / BLOCK_TEXTURE_ROW_COUNT;
	}
}

void chunk_debugFill(Chunk *chunk) {
	for (int dx = 0; dx < CHUNK_SIZE; dx++) {
		for (int dz = 0; dz < CHUNK_SIZE; dz++) {
			for (int dy = 0; dy < CHUNK_SIZE; dy++) {
				int y = chunk->y + dy;
				if (y < 64) {
					int block_id = y == 63 ? block_id_of("grass_block") : block_id_of("dirt");
					block_setId(CHUNK_BLOCK(chunk->blocks, chunk->x+dx, y, chunk->z+dz), block_id);
				}
			}
		}
	}

	glGenVertexArrays(1, &chunk->VAO);
	glGenBuffers(1, &chunk->VBO);

	// generate vertex data
	int vertex_idx = 0;
	chunk->vertex_count = 0;
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				Block block = *CHUNK_BLOCK(chunk->blocks, x, y, z);
				if (block_getId(block) == 0) { continue; }

				for (int f = 0; f < 6; f++) {
					// Face Culling
					if ((f == 0 && z > 0 && block_getId(*CHUNK_BLOCK(chunk->blocks, x, y, z-1)) != 0) ||
					    (f == 1 && z < CHUNK_SIZE-1 && block_getId(*CHUNK_BLOCK(chunk->blocks, x, y, z+1)) != 0) ||
					    (f == 2 && y > 0 && block_getId(*CHUNK_BLOCK(chunk->blocks, x, y-1, z)) != 0) ||
					    (f == 3 && y < CHUNK_SIZE-1 && block_getId(*CHUNK_BLOCK(chunk->blocks, x, y+1, z)) != 0) ||
					    (f == 4 && x > 0 && block_getId(*CHUNK_BLOCK(chunk->blocks, x-1, y, z)) != 0) ||
					    (f == 5 && x < CHUNK_SIZE-1 && block_getId(*CHUNK_BLOCK(chunk->blocks, x+1, y, z)) != 0)) {
						continue;
					}
					struct vertex face[6];
					generate_face(f, face, x, y, z, block_getId(block));
					memcpy(vertices+vertex_idx, face, sizeof(face));
					vertex_idx += 6;
				}
			}
		}
	}

	glBindVertexArray(chunk->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, chunk->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_idx*sizeof(struct vertex), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)sizeof(vec3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)(sizeof(vec3)*2));
	glEnableVertexAttribArray(2);

	chunk->vertex_count = vertex_idx;
}
