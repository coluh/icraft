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
	setPlayer(player);
	player_setPos(player, -10, 24, -5);
	player_rotateHead(player, (float[]){0, 1, 0}, -0.1);
	camera_attach(cam, player);
	input_setCallbacks(player, DEFAULT_PLAYER_KEYMAPS, 7);

	sceneManager_switchTo("InGame GUI");

	bool running = true;
	SDL_Event event;
	int frame_time = 1000 / render_getFPS();
	while (running) {
		Uint32 begin_time = SDL_GetTicks();

		/* handle input */
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { running = false; }
			input_handle(&event);
		}
		input_update();

		/* update world */
		camera_update(cam);
		// step();
		world_updateChunks(UNPACK3(player_getPos(player)));

		/* render */
		render(cam);

		Uint32 end_time = SDL_GetTicks();
		if (end_time - begin_time < frame_time) {
			SDL_Delay(frame_time - (end_time - begin_time));
		}
	}
}
