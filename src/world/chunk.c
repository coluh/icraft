#include "chunk.h"
#include <glad/glad.h>
#include <string.h>
#include "../../third_party/cglm/include/cglm/cglm.h"
#include "block/block.h"
#include "../util/mem.h"

#define CHUNK_MAX_VERTICE_COUNT (CHUNK_VOLUME*6*6)

static struct vertex {
	vec3 position;
	vec3 normal;
	vec2 uv;
} vertices[CHUNK_MAX_VERTICE_COUNT];

// -z, z, -y, y, -x, x
static void generate_face(int f, struct vertex face[], int x, int y, int z, BlockID id) {
	static const int indices[6][6][3] = {
		{
			{1, 0, 0}, {0, 0, 0}, {0, 1, 0},
			{1, 0, 0}, {0, 1, 0}, {1, 1, 0},
		},
		{
			{0, 0, 1}, {1, 0, 1}, {1, 1, 1},
			{0, 0, 1}, {1, 1, 1}, {0, 1, 1},
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
			{0, 0, 0}, {0, 0, 1}, {0, 1, 1},
			{0, 0, 0}, {0, 1, 1}, {0, 1, 0},
		},
		{
			{1, 0, 1}, {1, 0, 0}, {1, 1, 0},
			{1, 0, 1}, {1, 1, 0}, {1, 1, 1},
		},
	};
	static const int norms[6][3] = {
		{0, 0, -1}, {0, 0, 1},
		{0, -1, 0}, {0, 1, 0},
		{-1, 0, 0}, {1, 0, 0},
	};  // xyz, xzy, yxz, yzx, zxy, zyx
	static const float uvs[6][2] = {
		// {0, 0}, {1, 0}, {1, 1},
		// {0, 0}, {1, 1}, {0, 1},
		{0.0625, 0.0625}, {0.9375, 0.0625}, {0.9375, 0.9375},
		{0.0625, 0.0625}, {0.9375, 0.9375}, {0.0625, 0.9375},
	};

	int t = block_get(id)->textures[f];
	// tow triangles
	for (int i = 0; i < 6; i++) {
		face[i].position[0] = x + indices[f][i][0];
		face[i].position[1] = y + indices[f][i][1];
		face[i].position[2] = z + indices[f][i][2];
		face[i].normal[0] = norms[f][0];
		face[i].normal[1] = norms[f][1];
		face[i].normal[2] = norms[f][2];
		face[i].uv[0] = (float)(uvs[i][0] + t % BLOCK_TEXTURE_ROW_COUNT) / BLOCK_TEXTURE_ROW_COUNT;
		face[i].uv[1] = (float)(uvs[i][1] + (int)(t / BLOCK_TEXTURE_ROW_COUNT)) / BLOCK_TEXTURE_ROW_COUNT;
	}
}

void chunk_generateVertex(Chunk *chunk, Chunk *nearbys[6]) {

	glGenVertexArrays(1, &chunk->VAO);
	glGenBuffers(1, &chunk->VBO);

	// generate vertex data
	int vertex_idx = 0;
	chunk->vertex_count = 0;
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				BlockID id = chunk->blocks[x][y][z];
				if (id == BLOCK_Air) { continue; }

				for (int f = 0; f < 6; f++) {

					// Face Culling
					if ((f == 0 && z > 0 && chunk->blocks[x][y][z-1] != BLOCK_Air) ||
					    (f == 1 && z < CHUNK_SIZE-1 && chunk->blocks[x][y][z+1] != BLOCK_Air) ||
					    (f == 2 && y > 0 && chunk->blocks[x][y-1][z] != BLOCK_Air) ||
					    (f == 3 && y < CHUNK_SIZE-1 && chunk->blocks[x][y+1][z] != BLOCK_Air) ||
					    (f == 4 && x > 0 && chunk->blocks[x-1][y][z] != BLOCK_Air) ||
					    (f == 5 && x < CHUNK_SIZE-1 && chunk->blocks[x+1][y][z] != BLOCK_Air)) {
						continue;
					}
					if (nearbys[f]) {
						if ((f == 0 && z == 0 && nearbys[f]->blocks[x][y][CHUNK_SIZE-1] != BLOCK_Air) ||
						    (f == 1 && z == CHUNK_SIZE-1 && nearbys[f]->blocks[x][y][0] != BLOCK_Air) ||
						    (f == 2 && y == 0 && nearbys[f]->blocks[x][CHUNK_SIZE-1][z] != BLOCK_Air) ||
						    (f == 3 && y == CHUNK_SIZE-1 && nearbys[f]->blocks[x][0][z] != BLOCK_Air) ||
						    (f == 4 && x == 0 && nearbys[f]->blocks[CHUNK_SIZE-1][y][z] != BLOCK_Air) ||
						    (f == 5 && x == CHUNK_SIZE-1 && nearbys[f]->blocks[0][y][z] != BLOCK_Air)) {
							continue;
						}
					}

					struct vertex face[6];
					generate_face(f, face, x, y, z, id);
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

Chunk *newChunk(int x, int y, int z) {
	Chunk *c = zalloc(1, sizeof(Chunk));
	c->x = x;
	c->y = y;
	c->z = z;

	glm_mat4_identity(c->model);
	glm_translate(c->model, (vec3){x, y, z});

	c->generated = false;
	c->dirty = true;

	return c;
}
