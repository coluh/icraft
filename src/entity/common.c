#include "common.h"
#include <math.h>
#include "bodies.h"
#include "entity.h"
#include "../world/world.h"
#include "../game.h"

extern Game g;

float widthof(EntityType type) {
	switch (type) {
	case ENTITY_player:
		return PLAYER_WIDTH;
	case ENTITY_item:
		return ITEM_WIDTH;
	default:
		return 0;
	}
}

float heightof(EntityType type) {
	switch (type) {
	case ENTITY_player:
		return PLAYER_HEIGHT;
	case ENTITY_item:
		return ITEM_WIDTH;
	default:
		return 0;
	}
}

float thicknessof(EntityType type) {
	switch (type) {
	case ENTITY_player:
		return PLAYER_WIDTH;
	case ENTITY_item:
		return ITEM_WIDTH;
	default:
		return 0;
	}
}

void common_move_slide_gravity(Entity *self, World *world) {
	self->velocity.y += world->g * g.update_delta;
	common_move_slide(self, world);
}

void common_move_slide(Entity *self, World *world) {
	const float bw = widthof(self->type);
	const float bh = heightof(self->type);
	const float bt = thicknessof(self->type);

	V3 newpos = self->position;

	if (self->velocity.y != 0) {
		newpos.y += self->velocity.y * g.update_delta;
		if (world_collide(world, BODYP(newpos, bw, bh, bt))) {
			if (self->velocity.y > 0) {
				// this assumes that the border is at integer coord, or say all body is normal blocks
				newpos.y = floorf(newpos.y + bh/2) - bh/2;
			} else {
				newpos.y = ceilf(newpos.y - bh/2) + bh/2;
				self->on_ground = true;
				// if block_below.height is not integer...
			}
			self->velocity.y = 0;
		}
	}

	if (self->velocity.x != 0) {
		newpos.x += self->velocity.x * g.update_delta;
		if (world_collide(world, BODYP(newpos, bw, bh, bt))) {
			if (self->velocity.x > 0) {
				newpos.x = floorf(newpos.x + bw/2) - bw/2;
			} else {
				newpos.x = ceilf(newpos.x - bw/2) + bw/2;
			}
			self->velocity.x = 0;
		}
	}

	if (self->velocity.z != 0) {
		newpos.z += self->velocity.z * g.update_delta;
		if (world_collide(world, BODYP(newpos, bw, bh, bt))) {
			if (self->velocity.z > 0) {
				newpos.z = floorf(newpos.z + bt/2) - bt/2;
			} else {
				newpos.z = ceilf(newpos.z - bt/2) + bt/2;
			}
			self->velocity.z = 0;
		}
	}

	self->position = newpos;
}
