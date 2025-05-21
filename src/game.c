#include "game.h"
#include "util/log.h"
#include "render/window.h"
#include "render/font.h"
#include "render/resource.h"
#include "render/render.h"
#include "util/props.h"
#include "world/block/block.h"
#include "entity/entity.h"

#include "scene/scenemanager.h"
#include "world/world.h"
#include "camera/camera.h"
#include "entity/player.h"
#include "timer/timer.h"

#include <SDL2/SDL.h>

Game g;

void game_init() {
	log_init();
	window_init("Minecraft");
	font_init("assets/fonts/FandolHei-Regular.otf");
	resource_init();
	render_init();
	props_init();
	block_init();

	g.update_delta = 0.05f;
	g.entities = newEntityList();
	g.world = newWorld();
	g.camera = newCamera((float[]){0, 0, 0}, (float)g.window->width/g.window->height, CameraType_FPS);
	g.player_ref = entity_create(Entity_PLAYER, (V3){-10, 24, -5}, g.entities);

	entity_create(Entity_DROPS, (V3){0, 25, 0}, g.entities);
}

void game_loop() {

	camera_attach(g.camera, entity_get(g.entities, g.player_ref));
	// input_setCallbacks(g.player, DEFAULT_PLAYER_KEYMAPS, 7);

	sceneManager_push("Main HUD");

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
			// if this called update function, it will impact game logic
			// so do not call logic-related functions in this function
			sceneManager_handle(&event);
		}
		player_clearInput(entity_get(g.entities, g.player_ref));
		camera_updateRot(g.camera);

		accumulator += frame_time;
		while (accumulator >= g.update_delta) {

			/* update game */
			sceneManager_update();
			entity_update(g.entities, g.world);
			camera_updatePos(g.camera);
			world_updateChunks(g.world, UNPACK_XYZ(entity_get(g.entities, g.player_ref)->position));
			timer_update();

			accumulator -= g.update_delta;
		}

		/* render game */
		float alpha = accumulator / g.update_delta;
		render(g.camera, g.entities, g.world, alpha);
	}
}
