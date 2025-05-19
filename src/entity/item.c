#include "item.h"
#include "common.h"
#include "entity.h"
#include "../game.h"
#include <math.h>
#include "../../third_party/cglm/include/cglm/quat.h"
#include "../render/resource.h"
#include "../render/gl.h"

#define ITEM_FLOAT_RANGE 0.1f
#define ITEM_FLOAT_SPEED 3.0f
#define ITEM_ROTATE_SPEED 2.0f

extern Game g;

static void topre(ItemData *id) {
	for (int i = 0; i < 3; i++) {
		id->previous_position[i] = id->render_position[i];
	}
	for (int i = 0; i < 4; i++) {
		id->previous_rotation[i] = id->render_rotation[i];
	}
}

void item_init(Entity *entity) {
	ItemData *item = &entity->item;
	item->scale = 0.2;
	item->render_position[0] = entity->position.x;
	item->render_position[1] = entity->position.y;
	item->render_position[2] = entity->position.z;
	item->render_rotation[0] = entity->rotation[0];
	item->render_rotation[1] = entity->rotation[1];
	item->render_rotation[2] = entity->rotation[2];
	item->render_rotation[3] = entity->rotation[3];
	topre(item);
}

void item_update(Entity *self, World *w) {
	ItemData *item = &self->item;

	topre(item);

	item->render_position[0] = self->position.x;
	item->render_position[1] = self->position.y + (sinf(item->float_timer) + 1.0f) * ITEM_FLOAT_RANGE;
	item->render_position[2] = self->position.z;
	vec3 up = {0, 1, 0};
	versor rot;
	glm_quatv(rot, item->rotate_timer, up);
	glm_quat_mul(rot, self->rotation, item->render_rotation);

	item->float_timer += ITEM_FLOAT_SPEED * g.update_delta;
	item->rotate_timer += ITEM_ROTATE_SPEED * g.update_delta;

	common_move_slide_gravity(self, w);
}

void item_render(Entity *entity, float alpha) {

	ItemData *item = &entity->item;
	vec3 position;
	glm_vec3_lerp(item->previous_position, item->render_position, alpha, position);
	versor rotation;
	glm_quat_lerp(item->previous_rotation, item->render_rotation, alpha, rotation);

	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, position);
	mat4 rot;
	glm_quat_mat4(rotation, rot);
	glm_mat4_mul(model, rot, model);
	glm_scale_uni(model, item->scale);

	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)model);
	glBindVertexArray(g.res->meshes.cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, g.res->meshes.cubeVAO_count);
}
