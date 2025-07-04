#include "render_3d.h"
#include "../game.h"
#include "resource.h"
#include "../world/chunk.h"
#include "gl.h"
#include "../entity/entity.h"
#include "../util/log.h"
#include "../util/props.h"
// #include "../../third_party/cglm/include/cglm/affine-pre.h"
#include "../../third_party/cglm/include/cglm/cglm.h"
#include "texture.h"

extern Game g;

static void renderDestroyingTexture(float time, float total_time, int x, int y, int z) {

	int p = ((float)time / total_time) * 10;
	if (p > 9) p = 9;
	glUniform1i(g.res->shaders.basic_location.use_uv_offset, 1);
	p += BLOCK_TEXTURE_ROW_COUNT * (BLOCK_TEXTURE_ROW_COUNT - 1); // destroy texture position
	float uv[2];
	texture_blockUVoffset(p, uv);
	glUniform2f(g.res->shaders.basic_location.uv_offset, uv[0], uv[1]);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -1.0f);

	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){x, y, z});
	// cubeVAO is originally for item, it's center is in 0,0,0
	glm_translate(model, (vec3){0.5f, 0.5f, 0.5f});
	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)model);
	glBindVertexArray(g.res->meshes.cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, g.res->meshes.cubeVAO_count);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glUniform1i(g.res->shaders.basic_location.use_uv_offset, 0);
}



static int water_vertices(const float heights[4], Vertex vertices[], const int no_face[]) {
	V3 block[8] = {
		{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1},
		{0, heights[0], 0}, {1, heights[1], 0}, {1, heights[2], 1}, {0, heights[3], 1},
	};
	V3 norms[6] = {{0, -1, 0}, {0, 0, -1}, {1, 0, 0}, {0, 0, 1}, {-1, 0, 0}, {0, 1, 0}}; // TODO: up face norm
	V2 uvs[4] = {{0, 0}, {1/16.0f, 0}, {1/16.0f, 1/16.0f}, {0, 1/16.0f}};

	int idx = 0;

	if (!no_face[0]) {
		vertices[idx++] = (Vertex){ .pos = block[0], .norm = norms[0], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[1], .norm = norms[0], .uv = uvs[1]};
		vertices[idx++] = (Vertex){ .pos = block[2], .norm = norms[0], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[0], .norm = norms[0], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[2], .norm = norms[0], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[3], .norm = norms[0], .uv = uvs[3]};
	}

	if (!no_face[1]) {
		vertices[idx++] = (Vertex){ .pos = block[0], .norm = norms[1], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[4], .norm = norms[1], .uv = uvs[1]};
		vertices[idx++] = (Vertex){ .pos = block[5], .norm = norms[1], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[0], .norm = norms[1], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[5], .norm = norms[1], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[1], .norm = norms[1], .uv = uvs[3]};
	}

	if (!no_face[2]) {
		vertices[idx++] = (Vertex){ .pos = block[1], .norm = norms[2], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[5], .norm = norms[2], .uv = uvs[1]};
		vertices[idx++] = (Vertex){ .pos = block[6], .norm = norms[2], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[1], .norm = norms[2], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[6], .norm = norms[2], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[2], .norm = norms[2], .uv = uvs[3]};
	}

	if (!no_face[3]) {
		vertices[idx++] = (Vertex){ .pos = block[2], .norm = norms[3], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[6], .norm = norms[3], .uv = uvs[1]};
		vertices[idx++] = (Vertex){ .pos = block[7], .norm = norms[3], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[2], .norm = norms[3], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[7], .norm = norms[3], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[3], .norm = norms[3], .uv = uvs[3]};
	}

	if (!no_face[4]) {
		vertices[idx++] = (Vertex){ .pos = block[3], .norm = norms[4], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[7], .norm = norms[4], .uv = uvs[1]};
		vertices[idx++] = (Vertex){ .pos = block[4], .norm = norms[4], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[3], .norm = norms[4], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[4], .norm = norms[4], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[0], .norm = norms[4], .uv = uvs[3]};
	}

	if (!no_face[5]) {
		vertices[idx++] = (Vertex){ .pos = block[4], .norm = norms[5], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[7], .norm = norms[5], .uv = uvs[1]};
		vertices[idx++] = (Vertex){ .pos = block[6], .norm = norms[5], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[4], .norm = norms[5], .uv = uvs[0]};
		vertices[idx++] = (Vertex){ .pos = block[6], .norm = norms[5], .uv = uvs[2]};
		vertices[idx++] = (Vertex){ .pos = block[5], .norm = norms[5], .uv = uvs[3]};
	}

	return idx;
}

static void renderWater(int x, int y, int z, int level, const World *w) {
	// TODO: render half-transparent correctly
	glUniform1i(g.res->shaders.basic_location.use_uv_offset, 1);
	float uv[2];
	texture_blockUVoffset(WATER_TEXTURE_INDEX, uv);
	glUniform2f(g.res->shaders.basic_location.uv_offset, uv[0], uv[1]);

	// TODO: water flowing down and water near source are all of level 7, but they have different height
	// surrounding levels
	static const int dirs[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
	int levels[8] = { 0 };
	for (int d = 0; d < 8; d++) {
		int nx = x + dirs[d][0];
		int nz = z + dirs[d][1];
		BlockState *s = blockstate_getByType(w, nx, y, nz, BlockState_WATER);
		if (s && s->type == BlockState_WATER) {
			levels[d] = s->water.level;
		}
	}
	const int blocks[6] = {
		!BLOCK_ISTRANSPARENT(world_block(w, x, y - 1, z)),
		!BLOCK_ISTRANSPARENT(world_block(w, x, y, z - 1)),
		!BLOCK_ISTRANSPARENT(world_block(w, x + 1, y, z)),
		!BLOCK_ISTRANSPARENT(world_block(w, x, y, z + 1)),
		!BLOCK_ISTRANSPARENT(world_block(w, x - 1, y, z)),
		!BLOCK_ISTRANSPARENT(world_block(w, x, y + 1, z)),
	};
	float heights[4];
	if (level >= 7 || blockstate_getByType(w, x, y + 1, z, BlockState_WATER)) {
		heights[0] = heights[1] = heights[2] = heights[3] = 1.0f;
	} else {
		// from x-z-, clockwise
		heights[0] = AVG4(level, levels[4], levels[5], levels[6]) / 7.0f;
		heights[1] = AVG4(level, levels[6], levels[7], levels[0]) / 7.0f;
		heights[2] = AVG4(level, levels[0], levels[1], levels[2]) / 7.0f;
		heights[3] = AVG4(level, levels[2], levels[3], levels[4]) / 7.0f;
		if (blocks[4] && blocks[1])
			heights[0] = level/1.0f/7;
		if (!blocks[4] && blocks[1])
			heights[0] = (level + levels[4])/2.0f/7;
		if (blocks[4] && !blocks[1])
			heights[0] = (level + levels[6])/2.0f/7;
		if (blocks[1] && blocks[2])
			heights[1] = level/1.0f/7;
		if (!blocks[1] && blocks[2])
			heights[1] = (level + levels[6])/2.0f/7;
		if (blocks[1] && !blocks[2])
			heights[1] = (level + levels[0])/2.0f/7;
		if (blocks[2] && blocks[3])
			heights[2] = level/1.0f/7;
		if (!blocks[2] && blocks[3])
			heights[2] = (level + levels[0])/2.0f/7;
		if (blocks[2] && !blocks[3])
			heights[2] = (level + levels[2])/2.0f/7;
		if (blocks[3] && blocks[4])
			heights[3] = level/1.0f/7;
		if (!blocks[3] && blocks[4])
			heights[3] = (level + levels[2])/2.0f/7;
		if (blocks[3] && !blocks[4])
			heights[3] = (level + levels[4])/2.0f/7;

		if (levels[0] >= 7)
			heights[1] = heights[2] = 1.0f;
		if (levels[2] >= 7)
			heights[2] = heights[3] = 1.0f;
		if (levels[4] >= 7)
			heights[3] = heights[0] = 1.0f;
		if (levels[6] >= 7)
			heights[0] = heights[1] = 1.0f;
		if (levels[1] >= 7)
			heights[2] = 1.0f;
		if (levels[3] >= 7)
			heights[3] = 1.0f;
		if (levels[5] >= 7)
			heights[0] = 1.0f;
		if (levels[7] >= 7)
			heights[1] = 1.0f;
	}
	BlockState *bs = blockstate_getByType(w, x, y - 1, z, BlockState_WATER);
	const int no_face[6] = {
		(bs && bs->water.level >= 7) || blocks[0],
		levels[6] > 0 || blocks[1],
		levels[0] > 0 || blocks[2],
		levels[2] > 0 || blocks[3],
		levels[4] > 0 || blocks[4],
		blockstate_getByType(w, x, y + 1, z, BlockState_WATER) || blocks[5],
	};
	Vertex block_vertices[36];
	int vertex_count = water_vertices(heights, block_vertices, no_face);
	if (vertex_count == 0) {
		return;
	}
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_count*sizeof(Vertex), block_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
	
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){x, y, z});
	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)model);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);

	glUniform1i(g.res->shaders.basic_location.use_uv_offset, 0);
}

static void renderWorldWater(const World *w) {
	for (int i = 0; i < w->render_list.size; i++) {
		Chunk *c = w->render_list.chunks[i];
		if (!c->generated) {
			continue;
		}
		const BlockStateList *list = &c->block_states;
		for (int i = 0; i < list->length; i++) {
			const BlockStateNode *n = &list->data[i];
			if (n->state.type != BlockState_WATER) {
				continue;
			}
			int x = c->x + n->x;
			int y = c->y + n->y;
			int z = c->z + n->z;
			renderWater(x, y, z, n->state.water.level, w);
		}
	}
}

// things that do not need other chunks
static void threed_renderChunk(const Chunk *chunk) {

	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)chunk->model);
	glBindVertexArray(chunk->VAO);

	glEnable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLES, 0, chunk->opaque_vertex_count);
	glDisable(GL_CULL_FACE);

	glDepthMask(GL_FALSE);
	glDrawArrays(GL_TRIANGLES, chunk->opaque_vertex_count, chunk->vertex_count - chunk->opaque_vertex_count);
	glDepthMask(GL_TRUE);

	const BlockStateList *list = &chunk->block_states;
	for (int i = 0; i < list->length; i++) {
		const BlockStateNode *n = &list->data[i];
		int x = chunk->x + n->x;
		int y = chunk->y + n->y;
		int z = chunk->z + n->z;
		switch (n->state.type) {
		case BlockState_DESTROY:
			renderDestroyingTexture(n->state.destroy.time, n->state.destroy.total, x, y, z);
			break;
		default:
			break;
		}
	}
}

void threed_renderChunks(const World *world) {

	glActiveTexture(GL_TEXTURE0);  // no need, because tex0 is activated by default
	glBindTexture(GL_TEXTURE_2D, g.res->textures.blocks);

	for (int i = 0; i < world->render_list.size; i++) {
		const Chunk *chunk = world->render_list.chunks[i];
		if (!chunk->generated || chunk->VAO == 0) {
			// not generated; generated but vertex never built
			continue;
		}
		threed_renderChunk(world->render_list.chunks[i]);
	}

	renderWorldWater(world);
}

void threed_renderFacing() {
	const IV3 *b = &entity_get(g.entities, g.player_ref)->player.facing_block;
	if (world_block(g.world, b->x, b->y, b->z) == BLOCK_Air) {
		return;
	}
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){b->x, b->y, b->z});
	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)model);

	glLineWidth(5.0f);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.0f, -1.0f);
	glBindVertexArray(g.res->meshes.frameVAO);
	glDrawArrays(GL_LINES, 0, g.res->meshes.frameVAO_count);
	glDisable(GL_POLYGON_OFFSET_LINE);
}
