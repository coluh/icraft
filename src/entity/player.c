#include "player.h"
#include "bodies.h"
#include "common.h"
#include "entity.h"
#include <math.h>
#include "../world/world.h"
#include "../physics/collision.h"
#include "../game.h"
#include "../effects/particle.h"
#include "../util/props.h"
#include "../util/log.h"

#include "../../third_party/cglm/include/cglm/vec3.h"
#include "../../third_party/cglm/include/cglm/quat.h"

#define PLAYER_ROTATE_SENSI	0.003f
#define PLAYER_MOVE_SPEED	4.3f
#define PLAYER_JUMP_SPEED	10.0f

extern Game g;

void player_init(Entity *self) {
	PlayerData *p = &self->player;
	player_rotateHead(self, (float[]){0, 1, 0}, -0.1);
	glm_quat_identity(p->body_rotation);
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
	glm_vec3_add(right, front, hv); // we will not normalize here to keep the move feature
	self->velocity.x = hv[0] != 0 ? hv[0] * PLAYER_MOVE_SPEED : self->velocity.x;
	self->velocity.z = hv[2] != 0 ? hv[2] * PLAYER_MOVE_SPEED : self->velocity.z;

	if (p->input.jump) {
		if (self->on_ground) {
			self->velocity.y = PLAYER_JUMP_SPEED;
			self->on_ground = false;
		}
	}
	if (p->input.destroy) {
		int x = p->facing_block.x;
		int y = p->facing_block.y;
		int z = p->facing_block.z;
		if (world_block(w, x, y, z) != BLOCK_Air) {
			BlockState *state = blockstate_getByType(w, x, y, z, BlockState_DESTROY);
			if (state == NULL) {
				// start destroy
				float total_time = block_get(world_block(w, x, y, z))->break_time;
				BlockState s = { BlockState_DESTROY, .destroy = { .total = total_time } };
				state = blockstate_add(&s, w, x, y, z);
			}
			state->destroy.time += g.update_delta;
			state->destroy.focus = true;
			float minx, miny, minz, maxx, maxy, maxz;
			minx = floorf(p->facing_point.x);
			maxx = ceilf(p->facing_point.x);
			miny = floorf(p->facing_point.y);
			maxy = ceilf(p->facing_point.y);
			minz = floorf(p->facing_point.z);
			maxz = ceilf(p->facing_point.z);
			if (p->facing_block_face == Face_BACK || p->facing_block_face == Face_FRONT) {
				minz = maxz = floorf(p->facing_point.z + 0.5f);
			} else if (p->facing_block_face == Face_BOTTOM || p->facing_block_face == Face_TOP) {
				miny = maxy = floorf(p->facing_point.y + 0.5f);
			} else {
				minx = maxx = floorf(p->facing_point.x + 0.5f);
			}
			particle_spawn(rand_float(minx, maxx), rand_float(miny, maxy), rand_float(minz, maxz), block_get(world_block(w, x, y, z))->textures[p->facing_block_face]);
			if (state->destroy.time >= state->destroy.total) {
				// destroyed
				blockstate_removeByType(w, x, y, z, BlockState_DESTROY);

				// FIXME: move these code to other place!!
				BlockID origin = world_modifyBlock(w, x, y, z, BLOCK_Air);
				Entity *drops = entity_get(g.entities, entity_create(Entity_DROPS, (V3){x+0.5f, y+0.5f, z+0.5f}, g.entities));
				drops->drops.item.id = block_get(origin)->break_item;
				drops->velocity.x = rand_float(-2.0f, 2.0f);
				drops->velocity.y = rand_float(2.0f, 5.0f);
				drops->velocity.z = rand_float(-2.0f, 2.0f);
			}
		}
	}
	if (p->input.put) {
		Slot *slot = &p->inventory.hotbar[p->holding];
		if (slot->count > 0) { // no focus
			Item *item = &slot->item;
			ItemID id = item->id;
			// FIXME: move these code to other place!!
			if (item_putable(id)) {
				BlockID block = block_ofItem(id);
				if (world_block(w, p->facing_block.x, p->facing_block.y, p->facing_block.z) != BLOCK_Air &&
						world_block(w, p->putable_block.x, p->putable_block.y, p->putable_block.z) == BLOCK_Air) {
					Body *block_body = &(Body){p->putable_block.x, p->putable_block.y, p->putable_block.z, 1.0f, 1.0f, 1.0f};
					Body *player_body = BODYP(self->position, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH);
					if (!collision_overlap(block_body, player_body)) {
						// item -> block
						slot->count--;
						world_modifyBlock(w, p->putable_block.x, p->putable_block.y, p->putable_block.z, block);
						BlockState *s = blockstate_getByType(w, p->putable_block.x, p->putable_block.y, p->putable_block.z, BlockState_WATER);
						if (s && block_isCompleteSolid(block)) {
							blockstate_removeByType(w, p->putable_block.x, p->putable_block.y, p->putable_block.z, BlockState_WATER);
						}
					}
				}
			} else {
				// specific features
				switch (id) {
				case ITEM_Bucket:
					{
						vec3 eye = {self->position.x, self->position.y + PLAYER_EYE_OFFSET_Y, self->position.z};
						vec3 fr = {1, 0, 0};
						glm_quat_rotatev(self->rotation, fr, fr);
						glm_vec3_normalize(fr);
						const float u = 0.1f;
						vec3 uoff;
						glm_vec3_scale(fr, u, uoff);
						float d = 0.0f;
						while (true) {
							const BlockState *state = blockstate_getByType(w, floorf(eye[0]), floorf(eye[1]), floorf(eye[2]), BlockState_WATER);
							if (state && state->water.level == WATER_SOURCE) {
								blockstate_removeByType(w, floorf(eye[0]), floorf(eye[1]), floorf(eye[2]), BlockState_WATER);
								break;
							}
							glm_vec3_add(eye, uoff, eye);
							d += u;
							if (d >= 5.0f) {
								break;
							}
						}
					}
					break;
				case ITEM_WaterBucket:
					{
						int x = p->putable_block.x;
						int y = p->putable_block.y;
						int z = p->putable_block.z;
						if (blockstate_getByType(w, x, y, z, BlockState_WATER) == NULL) {
							// pour water
							BlockState s = { .type = BlockState_WATER, .water = { .level = WATER_SOURCE } };
							blockstate_add(&s, w, x, y, z);
						}
					}
					break;
				default:
					break;
				}
			}
		}
	}

	vec3 eye = {self->position.x, self->position.y + PLAYER_EYE_OFFSET_Y, self->position.z};
	float d = 0.0f;

	// TODO: for irregular surface
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
	vec3 ey;
	glm_vec3_copy(eye, ey);
	glm_vec3_sub(eye, uoff, eye);
	p->putable_block = (IV3){floorf(eye[0]), floorf(eye[1]), floorf(eye[2])};
	glm_vec3_lerp(eye, ey, 0.5, (float*)&p->facing_point); // TODO: be more precise
	if (p->putable_block.z < p->facing_block.z) {
		p->facing_block_face = Face_BACK;
	} else if (p->putable_block.z > p->facing_block.z) {
		p->facing_block_face = Face_FRONT;
	} else if (p->putable_block.y < p->facing_block.y) {
		p->facing_block_face = Face_BOTTOM;
	} else if (p->putable_block.y > p->facing_block.y) {
		p->facing_block_face = Face_TOP;
	} else if (p->putable_block.x < p->facing_block.x) {
		p->facing_block_face = Face_LEFT;
	} else if (p->putable_block.x > p->facing_block.x) {
		p->facing_block_face = Face_RIGHT;
	}

	common_move_slide_gravity(self, w);

	self->player.input.forward = 0;
	self->player.input.backward = 0;
	self->player.input.left = 0;
	self->player.input.right = 0;
	self->player.input.jump = false;
	self->player.input.destroy = false;
	self->player.input.put = false;
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
		if (slot->item.id == item.id && slot->count < 64) {
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
