#include "chunk.h"
#include <glad/glad.h>
#include <string.h>
#include "../../third_party/cglm/include/cglm/cglm.h"
#include "block/block.h"
#include "../util/mem.h"
#include "../render/texture.h"

#define CHUNK_MAX_VERTICE_COUNT (CHUNK_VOLUME*6*6)

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
		{0, 0}, {1, 0}, {1, 1},
		{0, 0}, {1, 1}, {0, 1},
		// {0.0625, 0.0625}, {0.9375, 0.0625}, {0.9375, 0.9375},
		// {0.0625, 0.0625}, {0.9375, 0.9375}, {0.0625, 0.9375},
		// {0.001, 0.001}, {0.999, 0.001}, {0.999, 0.999},
		// {0.001, 0.001}, {0.999, 0.999}, {0.001, 0.999},
	};

	// tow triangles
	for (int i = 0; i < 6; i++) {
		face[i].position[0] = x + indices[f][i][0];
		face[i].position[1] = y + indices[f][i][1];
		face[i].position[2] = z + indices[f][i][2];
		face[i].normal[0] = norms[f][0];
		face[i].normal[1] = norms[f][1];
		face[i].normal[2] = norms[f][2];
		texture_blockUVoffset(block_get(id)->textures[f], face[i].uv);
		face[i].uv[0] += uvs[i][0] / BLOCK_TEXTURE_ROW_COUNT;
		face[i].uv[1] += uvs[i][1] / BLOCK_TEXTURE_ROW_COUNT;
	}
}

static void generate_cross_face(struct vertex face[], int x, int y, int z, int textureIndex) {
	static const int indices[][3] = {
		{0, 0, 0}, {1, 0, 1}, {1, 1, 1},
		{0, 0, 0}, {1, 1, 1}, {0, 1, 0},
		{0, 0, 1}, {1, 0, 0}, {1, 1, 0},
		{0, 0, 1}, {1, 1, 0}, {0, 1, 1},
	};
	static const int norm[3] = {
		0, 1, 0,
	};
	static const float uvs[][2] = {
		{0, 0}, {1, 0}, {1, 1},
		{0, 0}, {1, 1}, {0, 1},
		{0, 0}, {1, 0}, {1, 1},
		{0, 0}, {1, 1}, {0, 1},
	};
	for (int i = 0; i < 12; i++) {
		face[i].position[0] = x + indices[i][0];
		face[i].position[1] = y + indices[i][1];
		face[i].position[2] = z + indices[i][2];
		face[i].normal[0] = norm[0];
		face[i].normal[1] = norm[1];
		face[i].normal[2] = norm[2];
		texture_blockUVoffset(textureIndex, face[i].uv);
		face[i].uv[0] += uvs[i][0] / BLOCK_TEXTURE_ROW_COUNT;
		face[i].uv[1] += uvs[i][1] / BLOCK_TEXTURE_ROW_COUNT;
	}
}

void chunk_generateVertex(Chunk *chunk, Chunk *nearbys[6]) {

	// generate vertex data
	int vertex_idx = 0;
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				BlockID id = chunk->blocks[x][y][z];
				if (id == BLOCK_Air) { continue; }
				if (block_isOpaqueBlock(id)) {
					for (int f = 0; f < 6; f++) {
						// Face Culling
						if ((f == 0 && z > 0 && chunk->blocks[x][y][z-1] != BLOCK_Air) ||
								(f == 1 && z < CHUNK_SIZE-1 && block_isOpaqueBlock(chunk->blocks[x][y][z+1])) ||
								(f == 2 && y > 0 && block_isOpaqueBlock(chunk->blocks[x][y-1][z])) ||
								(f == 3 && y < CHUNK_SIZE-1 && block_isOpaqueBlock(chunk->blocks[x][y+1][z])) ||
								(f == 4 && x > 0 && block_isOpaqueBlock(chunk->blocks[x-1][y][z])) ||
								(f == 5 && x < CHUNK_SIZE-1 && block_isOpaqueBlock(chunk->blocks[x+1][y][z]))) {
							continue;
						}
						if (nearbys[f]) {
							if ((f == 0 && z == 0 && block_isOpaqueBlock(nearbys[f]->blocks[x][y][CHUNK_SIZE-1])) ||
									(f == 1 && z == CHUNK_SIZE-1 && block_isOpaqueBlock(nearbys[f]->blocks[x][y][0])) ||
									(f == 2 && y == 0 && block_isOpaqueBlock(nearbys[f]->blocks[x][CHUNK_SIZE-1][z])) ||
									(f == 3 && y == CHUNK_SIZE-1 && block_isOpaqueBlock(nearbys[f]->blocks[x][0][z])) ||
									(f == 4 && x == 0 && block_isOpaqueBlock(nearbys[f]->blocks[CHUNK_SIZE-1][y][z])) ||
									(f == 5 && x == CHUNK_SIZE-1 && block_isOpaqueBlock(nearbys[f]->blocks[0][y][z]))) {
								continue;
							}
						}
						struct vertex face[6];
						generate_face(f, face, x, y, z, id);
						memcpy(vertices+vertex_idx, face, sizeof(face));
						vertex_idx += 6;
					}
				} else if (block_isPlant(id)) {
					struct vertex points[12];
					generate_cross_face(points, x, y, z, block_get(id)->textures[0]);
					memcpy(vertices + vertex_idx, points, sizeof(points));
					vertex_idx += 12;
				}
			}
		}
	}

	if (chunk->VAO == 0) {
		glGenVertexArrays(1, &chunk->VAO);
		glGenBuffers(1, &chunk->VBO);
		glBindVertexArray(chunk->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, chunk->VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)sizeof(vec3));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)(sizeof(vec3)*2));
		glEnableVertexAttribArray(2);
	}

	glBindBuffer(GL_ARRAY_BUFFER, chunk->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_idx*sizeof(struct vertex), vertices, GL_STATIC_DRAW);
	chunk->vertex_count = vertex_idx;
}
