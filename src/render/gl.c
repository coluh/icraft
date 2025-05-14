#include "gl.h"
#include "../game.h"
#include "resource.h"
#include "window.h"
#include "../util/log.h"

#include <glad/glad.h>
#include "../../third_party/cglm/include/cglm/cam.h"
#include "../../third_party/cglm/include/cglm/types.h"
#include <SDL2/SDL.h>

extern Game g;

void gl_init() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// GLenum err;
	// while ((err = glGetError()) != GL_NO_ERROR)
	// 	logw("21 OpenGL Error: %d", err);
	gl_setSize(g.window->width, g.window->height);
	glUseProgram(g.res->shaders.ui);
	glUniform1i(g.res->shaders.ui_location.useTexture, 0);
}

void gl_setSize(int width, int height) {
	glViewport(0, 0, width, height);
	mat4 proj2d;
	glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, proj2d);
	glUseProgram(g.res->shaders.ui);
	glUniformMatrix4fv(g.res->shaders.ui_location.proj, 1, GL_FALSE, (float*)proj2d);
}

void gl_clear(float r, float g, float b, float a) {
	glClearColor(0.4f, 0.8f, 1.0f, 1.0f); // good color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
