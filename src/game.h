#ifndef _ICRAFT_game_h
#define _ICRAFT_game_h

#include <stdbool.h>
#include "player/camera.h"
#include "player/player.h"
#include "world/world.h"

typedef struct Game {
	bool running;
	int fps;
	World *world;
	Player *player;
	Camera *camera;
} Game;

#define GAME_UPDATE_DT 0.05f

void game_init();

void game_loop();

#endif
