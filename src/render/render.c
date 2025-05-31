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
	gl_init();
	// GLenum err;
	// while ((err = glGetError()) != GL_NO_ERROR)
	// 	logw("1 OpenGL Error: %d", err);
	sceneManager_init();
}

void render(Camera *camera, const EntityList *entities, const World *world, float alpha) {
	gl_clear(0.4f, 0.8f, 1.0f, 1.0f);

	// 3D content
	camera_updateMatrix(camera, alpha);
	glUseProgram(g.res->shaders.basic);
	glUniformMatrix4fv(g.res->shaders.basic_location.view, 1, GL_FALSE, (float*)camera->view);
	glUniformMatrix4fv(g.res->shaders.basic_location.proj, 1, GL_FALSE, (float*)camera->proj);
	glUniformMatrix3fv(g.res->shaders.basic_location.normal_matrix, 1, GL_FALSE, (float*)(float[]){1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f});
	glBindTexture(GL_TEXTURE_2D, g.res->textures.blocks);
	threed_renderChunks(world);
	threed_renderFacing();
	entity_render(entities, alpha);

	// 2D content
	glUseProgram(g.res->shaders.ui);
	glDisable(GL_DEPTH_TEST);
	sceneManager_render();
	glEnable(GL_DEPTH_TEST);

	GL_CHECK();

	SDL_GL_SwapWindow(g.window->window);
}
