#include "player.h"
#include "bodies.h"
#include "common.h"
#include "entity.h"
#include <math.h>
#include "../world/world.h"

#include "../../third_party/cglm/include/cglm/vec3.h"
#include "../../third_party/cglm/include/cglm/quat.h"

#define PLAYER_ROTATE_SENSI	0.003f
#define PLAYER_MOVE_SPEED	5.0f
#define PLAYER_JUMP_SPEED	10.0f

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

void player_update(Entity *self, World *w) {

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

	vec3 eye = {self->position.x, self->position.y + PLAYER_EYE_OFFSET_Y, self->position.z};
	float d = 0.0f;

	vec3 fr = {1, 0, 0};
	glm_quat_rotatev(self->rotation, fr, fr);
	glm_vec3_normalize(fr);
	const float u = 0.1f;
	vec3 uoff;
	glm_vec3_scale(fr, u, uoff);
	while (world_block(w, eye[0], eye[1], eye[2]) == BLOCK_Air) {
		glm_vec3_add(eye, uoff, eye);
		d += u;
		if (d >= 5.0f) {
			break;
		}
	}
	p->facing_block = (IV3){floorf(eye[0]), floorf(eye[1]), floorf(eye[2])};

	common_move_slide_gravity(self, w);
}

void player_rotateHead(Entity *self, vec3 axis, float rad) {
	// since player_rotate is a callback for event while calling this, this
	// function better not impact the game logic too much
	versor rotation;
	glm_quatv(rotation, rad, axis);
	glm_quat_mul(rotation, self->rotation, self->rotation);
	glm_quat_normalize(self->rotation); // is this necessary?
}

// auto rotate, following head. you cannot rotate body without rotate head
static void player_rotateBody(Entity *self, vec3 axis, float rad) {
	versor rotation;
	glm_quatv(rotation, rad, axis);
	glm_quat_mul(rotation, self->player.body_rotation, self->player.body_rotation);
	glm_quat_normalize(self->player.body_rotation); // is this necessary?
}

// why void *p? cause this is a callback function
void player_rotate(void *p, SDL_Event *ev) {
	int dx = ev->motion.xrel;
	int dy = ev->motion.yrel;
	vec3 front, up, right;
	getv_upright(p, front, up, right);
	player_rotateHead(p, right, -dy * PLAYER_ROTATE_SENSI);

	vec3 fr = {1, 0, 0};
	glm_quat_rotatev(((Entity*)p)->rotation, fr, fr);
	if (fabsf(glm_vec3_dot(fr, up)) > 0.995f) {
		// back
		player_rotateHead(p, right, dy * PLAYER_ROTATE_SENSI);
	}

	/* vertical first, then horizontal */
	player_rotateHead(p, up, -dx * PLAYER_ROTATE_SENSI);

	// body only rotate around up vec
	// TODO: no no in minecraft body rotate following the walk direction not the head
	player_rotateBody(p, up, -dx * PLAYER_ROTATE_SENSI);
}

void player_pickup(Entity *self, Item item) {
	PlayerData *player = &self->player;
	Slot *slots = (Slot*)&player->inventory;
	for (int i = 0; i < PLAYER_FREESLOTS_COUNT; i++) {
		Slot *slot = &slots[i];
		// TODO: if (slot->item == item) // should ensure they are completely the same
		if (slot->item.id == item.id) {
			slot->count += 1;
			return;
		}
		if (slot->count == 0) {
			slot->item = item;
			slot->count += 1;
			return;
		}
	}
}
