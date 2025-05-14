#include "render.h"
#include "gl.h"
#include "render_3d.h"
#include "../entity/entity.h"
#include "../scene/scenemanager.h"
#include "../game.h"
#include "resource.h"
#include "window.h"
#include <SDL2/SDL.h>
#include "../util/log.h"

extern Game g;

void render_init() {
	// GLenum err;
	// while ((err = glGetError()) != GL_NO_ERROR)
	// 	logw("1 OpenGL Error: %d", err);
	gl_init();
	sceneManager_init();
}

void render(Camera *camera, World *world, float alpha) {
	gl_clear(0.4f, 0.8f, 1.0f, 1.0f);

	// 3D content
	glUseProgram(g.res->shaders.basic);
	camera_updateMatrix(camera, alpha);
	glUniformMatrix4fv(g.res->shaders.basic_location.view, 1, GL_FALSE, (float*)camera->view);
	glUniformMatrix4fv(g.res->shaders.basic_location.proj, 1, GL_FALSE, (float*)camera->proj);
	threed_renderChunks(camera, world);
	entity_render(alpha);

	// 2D content
	glUseProgram(g.res->shaders.ui);
	glDisable(GL_DEPTH_TEST);
	sceneManager_render();
	glEnable(GL_DEPTH_TEST);

	SDL_GL_SwapWindow(g.window->window);
}
