#include "common.h"
#include <math.h>
#include "bodies.h"
#include "entity.h"
#include "../game.h"
#include "../world/world.h"
#include "../physics/collision.h"
#include "../util/log.h"
#include "../util/props.h"

extern Game g;

float widthof(EntityType type) {
	switch (type) {
	case Entity_PLAYER:
		return PLAYER_WIDTH;
	case Entity_DROPS:
		return DROPS_WIDTH;
	default:
		return 0;
	}
}

float heightof(EntityType type) {
	switch (type) {
	case Entity_PLAYER:
		return PLAYER_HEIGHT;
	case Entity_DROPS:
		return DROPS_WIDTH;
	default:
		return 0;
	}
}

float thicknessof(EntityType type) {
	switch (type) {
	case Entity_PLAYER:
		return PLAYER_WIDTH;
	case Entity_DROPS:
		return DROPS_WIDTH;
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
	newpos.x += self->velocity.x * g.update_delta;
	newpos.y += self->velocity.y * g.update_delta;
	newpos.z += self->velocity.z * g.update_delta;
	IV3 bounding_min = {
		floorf(MIN(self->position.x, newpos.x) - bw/2),
		floorf(MIN(self->position.y, newpos.y) - bh/2),
		floorf(MIN(self->position.z, newpos.z) - bt/2),
	};
	IV3 bounding_max = {
		floorf(MAX(self->position.x, newpos.x) + bw/2),
		floorf(MAX(self->position.y, newpos.y) + bh/2),
		floorf(MAX(self->position.z, newpos.z) + bt/2),
	};
	int max_boxes = (bounding_max.x - bounding_min.x + 1) * (bounding_max.y - bounding_min.y + 1) * (bounding_max.z - bounding_min.z + 1);
	Body static_boxes[max_boxes];
	int n_static_boxes = 0;
	for (int i = bounding_min.x; i <= bounding_max.x; i++) {
		for (int j = bounding_min.y; j <= bounding_max.y; j++) {
			for (int k = bounding_min.z; k <= bounding_max.z; k++) {
				if (world_block(world, i, j, k) != BLOCK_Air) {
					static_boxes[n_static_boxes] = (Body){
						i, j, k, 1.0f, 1.0f, 1.0f
					};
					n_static_boxes++;
				}
			}
		}
	}

	float delta_time = g.update_delta;

	for (int loop = 0; ; loop++) {
		if (loop >= 16) {
			logw("Too much collision in one update // how do you do that");
			break;
		}

		Body *moving = BODYP(self->position, bw, bh, bt);
		float nearest_collide_time = delta_time;
		CollisionType nearest_collided = Collision_NONE;

		for (int i = 0; i < n_static_boxes; i++) {
			float collide_time = nearest_collide_time;
			CollisionType collided = collision_test(&static_boxes[i], moving, self->velocity, &collide_time);
			if (collided == Collision_NONE || collide_time >= nearest_collide_time) {
				continue;
			} else {
				nearest_collide_time = collide_time;
				nearest_collided = collided;
			}
		}

		if (nearest_collided == Collision_NONE) {
			self->position.x += self->velocity.x*delta_time;
			self->position.y += self->velocity.y*delta_time;
			self->position.z += self->velocity.z*delta_time;
			break;
		}

		if (nearest_collide_time < 0) { // instant collision
			if (nearest_collided == Collision_X) {
				self->position.x += self->velocity.x*nearest_collide_time;
			} else if (nearest_collided == Collision_Y) {
				self->position.y += self->velocity.y*nearest_collide_time;
			} else if (nearest_collided == Collision_Z) {
				self->position.z += self->velocity.z*nearest_collide_time;
			}
		} else {
			self->position.x += self->velocity.x*nearest_collide_time;
			self->position.y += self->velocity.y*nearest_collide_time;
			self->position.z += self->velocity.z*nearest_collide_time;
			delta_time -= nearest_collide_time;
		}

		if (nearest_collided == Collision_X) {
			self->velocity.x = 0;
		} else if (nearest_collided == Collision_Y) {
			if (self->velocity.y < 0) {
				self->on_ground = true;
			}
			self->velocity.y = 0;
		} else if (nearest_collided == Collision_Z) {
			self->velocity.z = 0;
		}

		if (delta_time < 0.001f) {
			break;
		}
	}
}
