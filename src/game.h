#ifndef _ICRAFT_game_h
#define _ICRAFT_game_h

#include <stdbool.h>
#include <stdio.h>
#include "util/types.h"

typedef struct Window Window;
typedef struct ResourceManager ResourceManager;
typedef struct SceneManager SceneManager;
typedef struct World World;
typedef struct EntityList EntityList;
typedef struct Player Player;
typedef struct Camera Camera;

typedef struct Game {
	bool running;
	int fps;
	float update_delta;
	int zoom_level; // = 1, 2, 3...
	Window *window;
	ResourceManager *res;
	SceneManager *scene_manager;
	World *world;
	EntityList *entities;
	PoolHandle player_ref;
	Camera *camera;
	FILE *logfile;
} Game;

void game_init();

void game_loop();

#endif
