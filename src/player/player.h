#ifndef _ICRAFT_player_player_h
#define _ICRAFT_player_player_h

#include "../../third_party/cglm/include/cglm/types.h"
#include <SDL2/SDL_events.h>

typedef struct Player Player;

Player *newPlayer();
float *player_getPos(Player *p);
void player_setPos(Player *p, float x, float y, float z);

// copy pos and head rot out
void player_copyTo(Player *p, vec3 pos, versor rot);

void player_move(Player *p, vec3 dir, float d);
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
