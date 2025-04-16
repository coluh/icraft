#include "render.h"
#include "gl.h"
#include "render_2d.h"
#include "render_3d.h"
#include "ui/scenemanager.h"
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

void render(Camera *camera) {
	gl_clear(UNPACK_RGBA(m.config.clearColor));

	// 3D content
	camera_updateMatrix(camera);
	threed_renderChunks(camera);

	// 2D content
	twod_setColor(0.0f, 0.0f, 0.0f, 0.5f);
	twod_drawQuad(50, 50, 100, 100);
	sceneManager_render();

	SDL_GL_SwapWindow(window_getWindow());
}
