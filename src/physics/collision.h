#ifndef _ICRAFT_physics_collision_h
#define _ICRAFT_physics_collision_h

#include "../util/types.h"
#include <stdbool.h>

typedef enum CollisionType {
	Collision_NONE, Collision_X, Collision_Y, Collision_Z,
} CollisionType;

bool collision_overlap(const Body *a, const Body *b);
CollisionType collision_test(const Body *stati, const Body *moving, V3 velocity, float *collide_time);

#endif
