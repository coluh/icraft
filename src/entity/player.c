#include "player.h"
#include "entity.h"
#include <math.h>
#include "../world/world.h"

#include "../../third_party/cglm/include/cglm/vec3.h"
#include "../../third_party/cglm/include/cglm/quat.h"

#define PLAYER_ROTATE_SENSI	0.003f
#define PLAYER_MOVE_SPEED	5.0f
#define PLAYER_JUMP_SPEED	10.0f

#define GAME_UPDATE_DT 0.05f

void player_init(Entity *self) {
	PlayerData *p = &self->player;
	player_rotateHead(self, (float[]){0, 1, 0}, -0.1);
	glm_quat_identity(p->body_rotation);
}

void player_clearInput(Entity *self) {
	self->player.input.forward = 0;
	self->player.input.backward = 0;
	self->player.input.left = 0;
	self->player.input.right = 0;
}

static void getv_upright(Entity *e, vec3 front, vec3 up, vec3 right) {

	// get forward vector
	front[0] = 1;
	front[1] = 0;
	front[2] = 0;
	glm_quat_rotatev(e->rotation, front, front);

	// get upright front
	front[1] = 0;
	glm_vec3_normalize(front);

	// get upright up
	up[0] = 0;
	up[1] = 1;
	up[2] = 0;

	// get upright right
	glm_vec3_crossn(front, up, right);
}

#define PLAYER_BODY(pos) BODY(pos.x-PLAYER_WIDTH/2,pos.y-PLAYER_HEIGHT/2,pos.z-PLAYER_WIDTH/2, \
		PLAYER_WIDTH,PLAYER_HEIGHT,PLAYER_WIDTH)

void player_update(Entity *self, World *w) {
// void player_update(Player *p, World *w) {

	PlayerData *p = &self->player;
	vec3 input_dir = {p->input.right - p->input.left, 0, p->input.forward - p->input.backward};
	glm_vec3_normalize(input_dir);
	vec3 front, up, right;
	getv_upright(self, front, up, right);
	vec3 hv;
	glm_vec3_scale(right, input_dir[0], right);
	glm_vec3_scale(front, input_dir[2], front);
	glm_vec3_add(right, front, hv);
	self->velocity.x = hv[0] * PLAYER_MOVE_SPEED;
	self->velocity.z = hv[2] * PLAYER_MOVE_SPEED;

	if (p->input.jump) {
		p->input.jump = false;
		if (self->on_ground) {
			self->velocity.y = PLAYER_JUMP_SPEED;
			self->on_ground = false;
		}
	}

	self->velocity.y += w->g * GAME_UPDATE_DT;

	V3 new_pos = self->position;

	if (self->velocity.y != 0) {
		new_pos.y += self->velocity.y * GAME_UPDATE_DT;
		if (world_collide(w, PLAYER_BODY(new_pos))) {
			if (self->velocity.y > 0) {
				// this assumes that the border is at integer coord
				new_pos.y = floorf(new_pos.y + PLAYER_HEIGHT/2) - PLAYER_HEIGHT/2;
			} else {
				new_pos.y = ceilf(new_pos.y - PLAYER_HEIGHT/2) + PLAYER_HEIGHT/2;
				self->on_ground = true;
				// if block_below.height is not integer...
			}
			self->velocity.y = 0;
		}
	}

	if (self->velocity.x != 0) {
		new_pos.x += self->velocity.x * GAME_UPDATE_DT;
		if (world_collide(w, PLAYER_BODY(new_pos))) {
			if (self->velocity.x > 0) {
				new_pos.x = floorf(new_pos.x + PLAYER_WIDTH/2) - PLAYER_WIDTH/2;
			} else {
				new_pos.x = ceilf(new_pos.x - PLAYER_WIDTH/2) + PLAYER_WIDTH/2;
			}
			self->velocity.x = 0;
		}
	}

	if (self->velocity.z != 0) {
		new_pos.z += self->velocity.z * GAME_UPDATE_DT;
		if (world_collide(w, PLAYER_BODY(new_pos))) {
			if (self->velocity.z > 0) {
				new_pos.z = floorf(new_pos.z + PLAYER_WIDTH/2) - PLAYER_WIDTH/2;
			} else {
				new_pos.z = ceilf(new_pos.z - PLAYER_WIDTH/2) + PLAYER_WIDTH/2;
			}
			self->velocity.z = 0;
		}
	}

	self->position = new_pos;
}

void player_rotateHead(Entity *self, vec3 axis, float rad) {
	versor rotation;
	glm_quatv(rotation, rad, axis);
	glm_quat_mul(rotation, self->rotation, self->rotation);
	glm_quat_normalize(self->rotation); // is this necessary?
}

// auto rotate following head. you cannot rotate body without rotate head
static void player_rotateBody(Entity *self, vec3 axis, float rad) {
	versor rotation;
	glm_quatv(rotation, rad, axis);
	glm_quat_mul(rotation, self->player.body_rotation, self->player.body_rotation);
	glm_quat_normalize(self->player.body_rotation); // is this necessary?
}

void player_rotate(void *p, SDL_Event *ev) {
	int dx = ev->motion.xrel;
	int dy = ev->motion.yrel;
	vec3 front, up, right;
	getv_upright(p, front, up, right);
	player_rotateHead(p, right, -dy * PLAYER_ROTATE_SENSI);

	vec3 fr = {1, 0, 0};
	glm_quat_rotatev(((Entity*)p)->rotation, fr, fr);
	if (fabsf(glm_vec3_dot(fr, up)) > 0.99f) {
		// back
		player_rotateHead(p, right, dy * PLAYER_ROTATE_SENSI);
	}

	/* vertical first, then horizontal */
	player_rotateHead(p, up, -dx * PLAYER_ROTATE_SENSI);

	// body only rotate around up vec
	player_rotateBody(p, up, -dx * PLAYER_ROTATE_SENSI);
}

