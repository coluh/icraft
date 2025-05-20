#include "drops.h"
#include "common.h"
#include "entity.h"
#include "../game.h"
#include <math.h>
#include <stdatomic.h>
#include "../../third_party/cglm/include/cglm/quat.h"
#include "../render/resource.h"
#include "../render/gl.h"
#include "../world/block/block.h"

#define DROPS_FLOAT_RANGE 0.1f
#define DROPS_FLOAT_SPEED 3.0f
#define DROPS_ROTATE_SPEED 2.0f

extern Game g;

static void topre(DropsData *id) {
	for (int i = 0; i < 3; i++) {
		id->previous_position[i] = id->render_position[i];
	}
	for (int i = 0; i < 4; i++) {
		id->previous_rotation[i] = id->render_rotation[i];
	}
}

void drops_init(Entity *entity) {
	DropsData *drops = &entity->drops;
	drops->scale = 0.2;
	drops->render_position[0] = entity->position.x;
	drops->render_position[1] = entity->position.y;
	drops->render_position[2] = entity->position.z;
	drops->render_rotation[0] = entity->rotation[0];
	drops->render_rotation[1] = entity->rotation[1];
	drops->render_rotation[2] = entity->rotation[2];
	drops->render_rotation[3] = entity->rotation[3];
	topre(drops);
}

void drops_update(Entity *self, World *w) {
	DropsData *drops = &self->drops;

	topre(drops);

	drops->render_position[0] = self->position.x;
	drops->render_position[1] = self->position.y + (sinf(drops->float_timer) + 1.0f) * DROPS_FLOAT_RANGE;
	drops->render_position[2] = self->position.z;
	vec3 up = {0, 1, 0};
	versor rot;
	glm_quatv(rot, drops->rotate_timer, up);
	glm_quat_mul(rot, self->rotation, drops->render_rotation);

	drops->float_timer += DROPS_FLOAT_SPEED * g.update_delta;
	drops->rotate_timer += DROPS_ROTATE_SPEED * g.update_delta;

	common_move_slide_gravity(self, w);
}

void drops_render(Entity *entity, float alpha) {

	DropsData *drops = &entity->drops;
	vec3 position;
	glm_vec3_lerp(drops->previous_position, drops->render_position, alpha, position);
	versor rotation;
	glm_quat_lerp(drops->previous_rotation, drops->render_rotation, alpha, rotation);

	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, position);
	mat4 rot;
	glm_quat_mat4(rotation, rot);
	glm_mat4_mul(model, rot, model);
	glm_scale_uni(model, drops->scale);

	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)model);
	mat3 normal_matrix;
	glm_mat4_pick3(model, normal_matrix);
	glm_mat3_inv(normal_matrix, normal_matrix);
	glm_mat3_transpose(normal_matrix);
	glUniformMatrix3fv(g.res->shaders.basic_location.normal_matrix, 1, GL_FALSE, (float*)normal_matrix);

	glBindVertexArray(g.res->meshes.cubeVAO);

	glUniform1i(g.res->shaders.basic_location.use_uv_offset, 1);
	const int *textures = block_get(block_ofItem(drops->item))->textures;
	float uv[2];
	for (int f = 0; f < 6; f++) {
		const int texture = textures[f];
		uv[0] = (float)(texture % BLOCK_TEXTURE_ROW_COUNT) / BLOCK_TEXTURE_ROW_COUNT;
		uv[1] = (float)(int)(texture / BLOCK_TEXTURE_ROW_COUNT) / BLOCK_TEXTURE_ROW_COUNT;
		glUniform2f(g.res->shaders.basic_location.uv_offset, uv[0], uv[1]);
		glDrawArrays(GL_TRIANGLES, f * g.res->meshes.cubeVAO_count / 6, g.res->meshes.cubeVAO_count / 6);
	}

	glUniform1i(g.res->shaders.basic_location.use_uv_offset, 0);
}
