#ifndef _ICRAFT_game_h
#define _ICRAFT_game_h

#include <stdbool.h>
#include <stdio.h>

typedef struct Window Window;
typedef struct SceneManager SceneManager;
typedef struct World World;
typedef struct Player Player;
typedef struct Camera Camera;

typedef struct Game {
	bool running;
	int fps;
	Window *window;
	SceneManager *scene_manager;
	World *world;
	Player *player;
	Camera *camera;
	FILE *logfile;
} Game;

#define GAME_UPDATE_DT 0.05f

void game_init();

void game_loop();

#endif
