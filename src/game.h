#ifndef _ICRAFT_game_h
#define _ICRAFT_game_h

#include <stdbool.h>
#include <stdio.h>

typedef struct Window Window;
typedef struct ResourceManager ResourceManager;
typedef struct SceneManager SceneManager;
typedef struct World World;
typedef struct EntityList EntityList;
typedef struct Entity Entity;
typedef struct Player Player;
typedef struct Camera Camera;

typedef struct Game {
	bool running;
	int fps;
	float update_delta;
	Window *window;
	ResourceManager *res;
	SceneManager *scene_manager;
	World *world;
	EntityList *entities;
	Entity *player;
	Camera *camera;
	FILE *logfile;
} Game;

void game_init();

void game_loop();

#endif
