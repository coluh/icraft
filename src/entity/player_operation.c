#include "player_operation.h"
#include "player.h"
#include "entity.h"
#include "../world/blockstate/blockstate.h"
#include "../world/block.h"
#include "../world/world.h"
#include "../effects/particle.h"
#include "../util/props.h"
#include "bodies.h"
#include "../physics/collision.h"
#include "../../third_party/cglm/include/cglm/quat.h"
#include "../../third_party/cglm/include/cglm/vec3.h"
#include "../util/log.h"
#include "../game.h"

extern Game g;

void player_destroying(Entity *self, World *w) {
	PlayerData *p = &self->player;
	int x = p->facing_block.x;
	int y = p->facing_block.y;
	int z = p->facing_block.z;
	if (world_block(w, x, y, z) == BLOCK_Air) {
		return;
	}

	BlockState *state = blockstate_getByType(w, x, y, z, BlockState_DESTROY);
	if (state == NULL) {
		// start destroy
		float total_time = block_get(world_block(w, x, y, z))->break_time;
		BlockState s = { BlockState_DESTROY, .destroy = { .total = total_time } };
		state = blockstate_add(&s, w, x, y, z);
	}
	state->destroy.time += g.update_delta;
	state->destroy.focus = true;

	// particles effect
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

		BlockID origin = world_modifyBlock(w, x, y, z, BLOCK_Air);
		Entity *drops = entity_get(g.entities, entity_create(Entity_DROPS, (V3){x+0.5f, y+0.5f, z+0.5f}, g.entities));
		drops->drops.item.id = block_get(origin)->break_item;
		drops->velocity.x = rand_float(-2.0f, 2.0f);
		drops->velocity.y = rand_float(2.0f, 5.0f);
		drops->velocity.z = rand_float(-2.0f, 2.0f);
	}
}

void player_place(Entity *self, World *w) {
	PlayerData *p = &self->player;

	// priorities:
	// 	1. block operation, like opening crafting table
	// 	2. placement, like placing water
	// 	3. use item, like eating carrot

	Slot *slot = &p->inventory.hotbar[p->holding];
	if (slot->count == 0) {
		return; // TODO: right click operations
	}

	Item *item = &slot->item;
	ItemID id = slot->item.id;
	bool has_place = world_block(w, p->facing_block.x, p->facing_block.y, p->facing_block.z) != BLOCK_Air &&
				world_block(w, p->putable_block.x, p->putable_block.y, p->putable_block.z) == BLOCK_Air;
	Body *block_body = &(Body){p->putable_block.x, p->putable_block.y, p->putable_block.z, 1.0f, 1.0f, 1.0f}; // TODO: not this block
	Body *player_body = BODYP(self->position, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH);
	bool not_collide = !collision_overlap(block_body, player_body);

	if (item_putable(id) && has_place && not_collide) {
		BlockID block = block_ofItem(id);
		// item -> block
		slot->count--;
		world_modifyBlock(w, p->putable_block.x, p->putable_block.y, p->putable_block.z, block);

		// TODO: move to world update
		BlockState *s = blockstate_getByType(w, p->putable_block.x, p->putable_block.y, p->putable_block.z, BlockState_WATER);
		if (s && !BLOCK_ISWATERABLE(block)) {
			blockstate_removeByType(w, p->putable_block.x, p->putable_block.y, p->putable_block.z, BlockState_WATER);
		}
		return;
	}

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
			BlockState *s = blockstate_getByType(w, x, y, z, BlockState_WATER);
			if (s == NULL) {
				// pour water
				BlockState s = { .type = BlockState_WATER, .water = { .level = WATER_SOURCE } };
				blockstate_add(&s, w, x, y, z);
			} else {
				s->water.level = WATER_SOURCE;
			}
		}
		break;
	default:
		logi("bro try to use %s", item_name(item));
		break;
	}
}

void player_pickup(Entity *self, Item *item) {
	PlayerData *player = &self->player;
	Slot *slots = (Slot*)&player->inventory;
	for (int i = 0; i < PLAYER_FREESLOTS_COUNT; i++) {
		Slot *slot = &slots[i];
		// TODO: should ensure they are completely the same, not just id
		if (slot->item.id == item->id && slot->count < 64) {
			slot->count += 1;
			return;
		}
		if (slot->count == 0) {
			slot->item = *item;
			slot->count += 1;
			return;
		}
	}
}

void player_drop(Entity *self) {
	Entity *player = entity_get(g.entities, g.player_ref);
	Slot *slot = &player->player.inventory.hotbar[player->player.holding];
	if (slot->count == 0) {
		return;
	}

	Entity *drops = entity_get(g.entities, entity_create(Entity_DROPS, (V3){
				player->position.x, player->position.y + PLAYER_EYE_OFFSET_Y, player->position.z}, g.entities));
	drops->drops.item = slot->item;
	slot->count--;
	vec3 fr = {1, 0, 0};
	glm_quat_rotatev(player->rotation, fr, fr);
	glm_vec3_normalize(fr);
	glm_vec3_scale(fr, PLAYER_DROP_SPEED, fr);
	drops->velocity.x = fr[0];
	drops->velocity.y = fr[1];
	drops->velocity.z = fr[2];
	drops->drops.pickup_timer = 2.0f; // override
}
