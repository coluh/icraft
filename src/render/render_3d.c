#include "render_3d.h"
#include "../game.h"
#include "resource.h"
#include "../world/chunk.h"
#include "gl.h"
#include "../entity/entity.h"
#include "../util/log.h"
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

static void renderWater(int x, int y, int z, int level) {
	// TODO: render half-transparent correctly
	glUniform1i(g.res->shaders.basic_location.use_uv_offset, 1);
	float uv[2];
	texture_blockUVoffset(10, uv);
	glUniform2f(g.res->shaders.basic_location.uv_offset, uv[0], uv[1]);

	float height = 1.0f * level / 7;
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){x, y, z});
	glm_translate(model, (vec3){0.5f, 0.5f, 0.5f});
	glm_translate(model, (vec3){0.0f, (height-1.0f)/2, 0.0f});
	glm_scale(model, (vec3){1.0f, height, 1.0f});
	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)model);

	glBindVertexArray(g.res->meshes.cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, g.res->meshes.cubeVAO_count);

	glUniform1i(g.res->shaders.basic_location.use_uv_offset, 0);
}

static void threed_renderChunk(const Chunk *chunk) {
	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)chunk->model);
	glBindVertexArray(chunk->VAO);
	glDrawArrays(GL_TRIANGLES, 0, chunk->vertex_count);

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
		case BlockState_WATER:
			renderWater(x, y, z, n->state.water.level);
			break;
		default:
			break;
		}
	}
}

void threed_renderChunks(const World *world) {

	glActiveTexture(GL_TEXTURE0);  // no need, because tex0 is activated by default
	glBindTexture(GL_TEXTURE_2D, g.res->textures.blocks);

	for (const ChunkNode *p = world->chunks; p != NULL; p = p->next) {
		threed_renderChunk(p->chunk);
	}
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

	glLineWidth(1.0f);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.0f, -1.0f);
	glBindVertexArray(g.res->meshes.frameVAO);
	glDrawArrays(GL_LINES, 0, g.res->meshes.frameVAO_count);
	glDisable(GL_POLYGON_OFFSET_LINE);
}
