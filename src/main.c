#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include "./util/log.h"
#include "input/input.h"
#include "player/camera.h"
#include "render/render.h"
#include "render/window.h"
#include "player/player.h"

#include <SDL2/SDL.h>

static void init() {
	log_init();
	window_init(NULL);
	render_init();
	camera_init();
}

static void game_loop();
int main(int argc, char *argv[]) {

	init();
	game_loop();

	return 0;
}

static void game_loop() {

	Player *player = newPlayer(-10, 16, 0);

	bool running = true;
	SDL_Event event;
	int frame_time = 1000 / render_getFPS();
	while (running) {
		Uint32 begin_time = SDL_GetTicks();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { running = false; }
			input_handle(&event, player_getCamera(player));
		}

		input_update(player_getCamera(player));
		// step();
		render(player_getCamera(player));

		Uint32 end_time = SDL_GetTicks();
		if (end_time - begin_time < frame_time) {
			SDL_Delay(frame_time - (end_time - begin_time));
		}
	}
}
