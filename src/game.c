#include "game.h"
#include "util/log.h"
#include "render/window.h"
#include "render/font.h"
#include "render/render.h"
#include "util/props.h"

#include "scene/scenemanager.h"
#include "world/world.h"
#include "player/camera.h"
#include "player/player.h"

#include <SDL2/SDL.h>

Game g;

void game_init() {
	log_init();
	window_init(NULL);
	font_init("assets/fonts/FandolHei-Regular.otf");
	render_init();
	props_init();

	g.world = newWorld();
	g.camera = newCamera((float[]){0, 0, 0}, (float)window_getWidth()/window_getHeight(), CameraType_FPS);
	g.player = newPlayer();
}

void game_loop() {

	camera_attach(g.camera, g.player);
	// input_setCallbacks(g.player, DEFAULT_PLAYER_KEYMAPS, 7);

	sceneManager_push("InGame GUI");

	g.running = true;
	SDL_Event event;

	Uint32 last_time = SDL_GetTicks();
	float accumulator = 0.0f;

	float fps_timer = 0.0f;
	int frame_count = 0;

	while (g.running) {
		Uint32 now = SDL_GetTicks();
		float frame_time = (now - last_time) / 1000.0f;
		last_time = now;

		fps_timer += frame_time;
		frame_count += 1;
		if (fps_timer >= 1.0f) {
			g.fps = frame_count;
			fps_timer = 0.0f;
			frame_count = 0;
		}

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { g.running = false; }
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
				window_setSize(event.window.data1, event.window.data2);
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F11) {
				window_toggleFullscreen();
			}
			sceneManager_handle(&event);
		}
		player_clearInput(g.player);
		sceneManager_update();
		camera_updateRot(g.camera);

		accumulator += frame_time;
		while (accumulator >= GAME_UPDATE_DT) {

			/* update game */
			player_update(g.player, g.world);
			camera_updatePos(g.camera);
			world_updateChunks(g.world, UNPACK_XYZ(g.player->pos));

			accumulator -= GAME_UPDATE_DT;
		}

		/* render game */
		float alpha = accumulator / GAME_UPDATE_DT;
		render(g.camera, g.world, alpha);
	}
}
