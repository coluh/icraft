#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include "./util/log.h"
#include "input/input.h"
#include "player/camera.h"
#include "render/render.h"
#include "render/window.h"
#include "render/font.h"
#include "player/player.h"
#include "util/props.h"
#include "world/world.h"
#include "render/ui/scenemanager.h"

#include <SDL2/SDL.h>

#define GAME_UPDATE_DT 0.05f

static void init() {
	log_init();
	window_init(NULL);
	font_init("assets/fonts/FandolHei-Regular.otf");
	render_init();
	props_init();
	input_init();
}

static void game_loop();
int main(int argc, char *argv[]) {

	init();
	game_loop();

	return 0;
}

static void game_loop() {

	Camera *cam = newCamera((float[]){0, 0, 0}, (float)window_getWidth()/window_getHeight(), CameraType_FPS);
	Player *player = newPlayer();
	player_setPos(player, -10, 24, -5);
	player_rotateHead(player, (float[]){0, 1, 0}, -0.1);

	setPlayer(player);
	camera_attach(cam, player);
	input_setCallbacks(player, DEFAULT_PLAYER_KEYMAPS, 7);

	sceneManager_switchTo("InGame GUI");

	bool running = true;
	SDL_Event event;
	Uint32 last_time = SDL_GetTicks();
	float accumulator = 0.0f;
	while (running) {
		Uint32 now = SDL_GetTicks();
		float delta = (now - last_time) / 1000.0f;
		last_time = now;

		accumulator += delta;
		while (accumulator >= GAME_UPDATE_DT) {

			/* handle input */
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) { running = false; }
				input_handle(&event);
			}
			input_update();

			/* update world */
			camera_update(cam);
			player_update(player);
			world_updateChunks(UNPACK3(player_getPos(player)));

			accumulator -= GAME_UPDATE_DT;
		}

		/* render */
		render(cam);
	}
}
