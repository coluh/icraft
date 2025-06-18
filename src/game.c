#include "game.h"
#include "effects/particle.h"
#include "util/log.h"
#include "render/window.h"
#include "render/font.h"
#include "render/resource.h"
#include "render/render.h"
#include "util/props.h"
#include "world/block.h"
#include "entity/entity.h"

#include "scene/scenemanager.h"
#include "world/world.h"
#include "camera/camera.h"
#include "entity/player_operation.h"
#include "timer/timer.h"

#include <SDL2/SDL.h>

Game g;

void game_init() {
	log_init();
	window_init("Minecraft");
	block_init();
	font_init("assets/fonts/FandolHei-Regular.otf");
	resource_init();
	render_init();
	props_init();

	g.update_delta = 0.05f;
	g.entities = newEntityList();
	g.world = newWorld();
	g.camera = newCamera((float[]){0, 0, 0}, (float)g.window->width/g.window->height, CameraType_FPS);
	g.player_ref = entity_create(Entity_PLAYER, (V3){-10, 0, -5}, g.entities);

	player_pickup(entity_get(g.entities, g.player_ref), &(Item){ITEM_Bucket});
	player_pickup(entity_get(g.entities, g.player_ref), &(Item){ITEM_WaterBucket});
}

void game_loop() {

	camera_attach(g.camera, g.player_ref);
	// input_setCallbacks(g.player, DEFAULT_PLAYER_KEYMAPS, 7);

	sceneManager_push("Main HUD");
	// build render chunks list
	// world_update(g.world, UNPACK_XYZ(entity_get(g.entities, g.player_ref)->position));

	Entity *p = entity_get(g.entities, g.player_ref);
	p->position.y = world_topY(g.world, p->position.x, p->position.z) + 1;
	// p->position.y = 300;
	world_update(g.world, UNPACK_XYZ(entity_get(g.entities, g.player_ref)->position));

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
			if (event.type == SDL_QUIT) {
				g.running = false;
			} else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
				window_setSize(event.window.data1, event.window.data2);
			} else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F11) {
				window_toggleFullscreen();
			}
			// do not call logic-related functions in this function
			sceneManager_handle(&event);
		}
		// to make perspective change fluent
		camera_updateRot(g.camera);
		sceneManager_update(); // NOTE: do not update game logic here, mark flags instead

		accumulator += frame_time;
		while (accumulator >= g.update_delta) {

			/* update game */
			g.ticks++;
			// NOTE: now entity and world update order matters, maybe it should not
			entity_update(g.entities, g.world);
			camera_updatePos(g.camera); // copy attached entity pos to camera pos
			world_update(g.world, UNPACK_XYZ(entity_get(g.entities, g.player_ref)->position));
			timer_update();
			particle_update();

			accumulator -= g.update_delta;
		}

		/* render game */
		float alpha = accumulator / g.update_delta;
		render(g.camera, g.entities, g.world, alpha);
	}
}
