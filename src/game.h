#ifndef _ICRAFT_game_h
#define _ICRAFT_game_h

#include <stdbool.h>

typedef struct World World;
typedef struct Player Player;
typedef struct Camera Camera;
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
