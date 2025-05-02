#include "render.h"
#include "gl.h"
#include "render_3d.h"
#include "../scene/scenemanager.h"
#include "window.h"
#include "../util/props.h"

#include <SDL2/SDL.h>

static struct {
	struct {
		int fps;
		struct {
			float r, g, b, a;
		} clearColor;
	} config;
} m;

void render_init() {
	gl_init();
	m.config.fps = 60;
	PACK_RGBA(m.config.clearColor, 0.4f, 0.8f, 1.0f, 1.0f);
	sceneManager_init();
}

int render_getFPS() { return m.config.fps; }

void render(Camera *camera, World *world) {
	gl_clear(UNPACK_RGBA(m.config.clearColor));

	// 3D content
	camera_updateMatrix(camera);
	threed_renderChunks(camera, world);

	// 2D content
	sceneManager_render();

	SDL_GL_SwapWindow(window_getWindow());
}
