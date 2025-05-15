#ifndef _ICRAFT_player_player_h
#define _ICRAFT_player_player_h

#include <stdbool.h>
#include "../util/types.h"
#include "../../third_party/cglm/include/cglm/types.h"
#include <SDL2/SDL_events.h>

typedef struct PlayerData {
	struct {
		int forward, backward, left, right;
		bool jump;
	} input;
	versor body_rotation;
	IV3 facing_block;
} PlayerData;

typedef struct Entity Entity;
typedef struct World World;
void player_init(Entity *self);
void player_clearInput(Entity *self);
void player_update(Entity *self, World *world);

void player_rotateHead(Entity *self, vec3 axis, float rad);
// callbacks
void player_rotate(void *p, SDL_Event *ev);

#endif
