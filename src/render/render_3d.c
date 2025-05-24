#include "render_3d.h"
#include "../game.h"
#include "resource.h"
#include "../world/chunk.h"
#include "gl.h"
#include "../entity/entity.h"
#include "../util/log.h"
#include "../../third_party/cglm/include/cglm/cglm.h"

extern Game g;

static void threed_renderChunk(const Chunk *chunk) {
	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)chunk->model);
	glBindVertexArray(chunk->VAO);
	glDrawArrays(GL_TRIANGLES, 0, chunk->vertex_count);
}

void threed_renderChunks(const World *world) {

	glActiveTexture(GL_TEXTURE0);  // no need, because tex0 is activated by default
	glBindTexture(GL_TEXTURE_2D, g.res->textures.blocks);

	for (const ChunkNode *p = world->chunks; p != NULL; p = p->next) {
		threed_renderChunk(p->chunk);
	}
}

static void renderDestroyingTexture(const BlockExtra *be, BlockID block) {

	int p = ((float)be->destroying.time / block_get(block)->break_time) * 10;
	if (p > 9) p = 9;
	// logd("p = %d", p);
	glUniform1i(g.res->shaders.basic_location.use_uv_offset, 1);
	float uv[2] = {
		(float)p / BLOCK_TEXTURE_ROW_COUNT,
		(float)(BLOCK_TEXTURE_ROW_COUNT - 1) / BLOCK_TEXTURE_ROW_COUNT,
	};
	glUniform2f(g.res->shaders.basic_location.uv_offset, uv[0], uv[1]);
	glDepthMask(GL_FALSE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -1.0f);

	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){be->x, be->y, be->z});
	// cubeVAO is originally for item, it's center is in 0,0,0
	glm_translate(model, (vec3){0.5f, 0.5f, 0.5f});
	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)model);
	glBindVertexArray(g.res->meshes.cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, g.res->meshes.cubeVAO_count);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDepthMask(GL_TRUE);
	glUniform1i(g.res->shaders.basic_location.use_uv_offset, 0);
}

void threed_renderExtras(const World *world) {
	for (const ChunkNode *cnp = world->chunks; cnp != NULL; cnp = cnp->next) {
		const Chunk *chunk = cnp->chunk;
		for (const BlockExtraNode *bp = chunk->extras; bp != NULL; bp = bp->next) {
			const BlockExtra *be = bp->extra;
			if (be->type == BlockExtra_DESTROY) {
				renderDestroyingTexture(be, world_block(world, be->x, be->y, be->z));
			}
		}
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

	glLineWidth(3.0f);
	glBindVertexArray(g.res->meshes.frameVAO);
	glDrawArrays(GL_LINES, 0, g.res->meshes.frameVAO_count);
}
