#ifndef _ICRAFT_player_player_h
#define _ICRAFT_player_player_h

#include <stdbool.h>
#include "../util/types.h"
#include "../../third_party/cglm/include/cglm/types.h"
#include <SDL2/SDL_events.h>
#include "../item/item.h"

#define PLAYER_INVENTORY_COLOMN 9
// #define PLAYER_INVENTORY_ROW 3 // this cannot be other value, right?
#define PLAYER_FREESLOTS_COUNT (PLAYER_INVENTORY_COLOMN*4+1)

typedef struct PlayerData {
	struct {
		int forward, backward, left, right;
		bool jump;
		bool destroy;
		bool put;
	} input;
	versor body_rotation;
	IV3 facing_block;
	IV3 putable_block; // before the facing_block

	// TODO: Add Survival/Creative Gamemode!!


	struct {
		Slot hotbar[PLAYER_INVENTORY_COLOMN];
		Slot storage[3][PLAYER_INVENTORY_COLOMN];
		Slot offhand;
		Slot armor[4];
	} inventory;

	int holding; // index to hotbar
} PlayerData;

typedef struct Entity Entity;
typedef struct World World;
void player_init(Entity *self);
void player_update(Entity *self, World *world);

void player_rotateHead(Entity *self, vec3 axis, float rad);
// callbacks
void player_rotate(void *p, SDL_Event *ev);

void player_pickup(Entity *self, Item item);

#endif
