#include "collision.h"
#include "../util/props.h"

#define EPSILON 0.01f

bool collision_overlap(const Body *a, const Body *b) {
	if ((a->x + a->w > b->x + EPSILON) && (a->x + EPSILON < b->x + b->w) &&
			(a->y + a->h > b->y + EPSILON) && (a->y + EPSILON < b->y + b->h) &&
			(a->z + a->d > b->z + EPSILON) && (a->z + EPSILON < b->z + b->d)) {
		return true;
	}
	return false;
}

CollisionType collision_test(const Body *stati, const Body *moving, V3 velocity, float *collide_time) {

	const V3 bounding = {
		.x = MAX(stati->x + stati->w, moving->x + moving->w) - MIN(stati->x, moving->x),
		.y = MAX(stati->y + stati->h, moving->y + moving->h) - MIN(stati->y, moving->y),
		.z = MAX(stati->z + stati->d, moving->z + moving->d) - MIN(stati->z, moving->z),
	};
	const float total_time = *collide_time;

	if (velocity.y) {
		const float distance = bounding.y - (stati->h + moving->h);
		*collide_time = distance / ABS(velocity.y);
		const float dt = MAX(*collide_time, 0);
		if (*collide_time < total_time) { // we can hit the static in time
			const float inner_margin = MAX(-0.5f * stati->h, -2.0f); // only if overlap in this range we recover it
			if (((velocity.y > 0 && stati->y - (moving->y + moving->h) > inner_margin) ||
						(velocity.y < 0 && moving->y - (stati->y + stati->h) > inner_margin)) &&
					(MAX(moving->x + moving->w + velocity.x * dt, stati->x + stati->w) - MIN(moving->x + velocity.x * dt, stati->x) < moving->w + stati->w - EPSILON) &&
					(MAX(moving->z + moving->d + velocity.z * dt, stati->z + stati->d) - MIN(moving->z + velocity.z * dt, stati->z) < moving->d + stati->d - EPSILON)) {
				return Collision_Y;
			}
		} else {
			return Collision_NONE;
		}
	}

	if (velocity.x) {
		const float distance = bounding.x - (stati->w + moving->w);
		*collide_time = distance / ABS(velocity.x);
		const float dt = MAX(*collide_time, 0);
		if (*collide_time < total_time) {
			const float inner_margin = MAX(-0.5f * stati->w, -2.0f);
			if (((velocity.x > 0 && stati->x - (moving->x + moving->w) > inner_margin) ||
						(velocity.x < 0 && moving->x - (stati->x + stati->w) > inner_margin)) &&
					(MAX(moving->y + moving->h + velocity.y * dt, stati->y + stati->h) - MIN(moving->y + velocity.y * dt, stati->y) < moving->h + stati->h - EPSILON) &&
					(MAX(moving->z + moving->d + velocity.z * dt, stati->z + stati->d) - MIN(moving->z + velocity.z * dt, stati->z) < moving->d + stati->d - EPSILON)) {
				return Collision_X;
			}
		} else {
			return Collision_NONE;
		}
	}

	if (velocity.z) {
		const float distance = bounding.z - (stati->d + moving->d);
		*collide_time = distance / ABS(velocity.z);
		const float dt = MAX(*collide_time, 0);
		if (*collide_time < total_time) {
			const float inner_margin = MAX(-0.5f * stati->d, -2.0f);
			if (((velocity.z > 0 && stati->z - (moving->z + moving->d) > inner_margin) ||
						(velocity.z < 0 && moving->z - (stati->z + stati->d) > inner_margin)) &&
					(MAX(moving->x + moving->w + velocity.x * dt, stati->x + stati->w) - MIN(moving->x + velocity.x * dt, stati->x) < moving->w + stati->w - EPSILON) &&
					(MAX(moving->y + moving->h + velocity.y * dt, stati->y + stati->h) - MIN(moving->y + velocity.y * dt, stati->y) < moving->h + stati->h - EPSILON)) {
				return Collision_Z;
			}
		} else {
			return Collision_NONE;
		}
	}

	return Collision_NONE;
}

/*
bool world_collide(World *world, const Body *body, float *collide_boundary, int *collide_direction) {
	float x = body->x;
	float y = body->y;
	float z = body->z;
	float w = body->w;
	float h = body->h;
	float d = body->d;
	if (w == 0 && h == 0 && d == 0) {
		return false;
	}
	for (int i = floorf(x); i < ceilf(x + w); i++) {
		for (int j = floorf(y); j < ceilf(y + h); j++) {
			for (int k = floorf(z); k < ceilf(z + d); k++) {
				BlockID block = world_block(world, i, j, k);
				if (block == BLOCK_Air) {
					continue;
				}
				if (((x + w > i) && (x < i + 1.0f)) &&
					((y + h > j) && (y < j + 1.0f)) &&
					((z + d > k) && (z < k + 1.0f))) {
					collide_boundary[0] = x < i ? i : i + 1.0f;
					collide_boundary[1] = y < j ? j : j + 1.0f;
					collide_boundary[2] = z < k ? k : k + 1.0f;
					collide_direction[0] = x < i ? 1 : -1;
					collide_direction[1] = y < j ? 1 : -1;
					collide_direction[2] = z < k ? 1 : -1;
					return true;
				}
			}
		}
	}
	return false;
}
*/
