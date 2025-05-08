#ifndef _ICRAFT_player_player_h
#define _ICRAFT_player_player_h

#include "../world/world.h"
#include "../../third_party/cglm/include/cglm/types.h"
#include <SDL2/SDL_events.h>

typedef struct V3 {
	float x;
	float y;
	float z;
} V3;

typedef struct Player {
	struct {
		int forward, backward, left, right;
		bool jump;
	} input;
	V3 pos;
	V3 v;
	bool on_ground;
	versor brot; // body rotation
	versor hrot; // head rotation
} Player;

Player *newPlayer();

void player_clearInput(Player *p);
void player_update(Player *p, World *w);

void player_rotateHead(Player *p, vec3 axis, float rad);
// void player_rotateBody(Player *p, vec3 axis, float rad);

// callbacks
void player_forward(void *p, SDL_Event *ev);
void player_backward(void *p, SDL_Event *ev);
void player_left(void *p, SDL_Event *ev);
void player_right(void *p, SDL_Event *ev);
void player_up(void *p, SDL_Event *ev);
void player_down(void *p, SDL_Event *ev);
void player_rotate(void *p, SDL_Event *ev);

#endif
