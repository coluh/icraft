#include "render.h"
#include "gl.h"
#include "render_3d.h"
#include "../scene/scenemanager.h"
#include "../game.h"
#include "window.h"
#include <SDL2/SDL.h>

extern Game g;

void render_init() {
	gl_init();
	sceneManager_init();
}

void render(Camera *camera, World *world, float alpha) {
	gl_clear(0.4f, 0.8f, 1.0f, 1.0f);

	// 3D content
	camera_updateMatrix(camera, alpha);
	threed_renderChunks(camera, world);

	// 2D content
	sceneManager_render();

	SDL_GL_SwapWindow(g.window->window);
}
