#include "window.h"
#include "../util/log.h"
#include <SDL2/SDL_mouse.h>
#include <stdbool.h>

#include <glad/glad.h>
/* glad first */

#include <SDL2/SDL_video.h>
// #include <SDL2/SDL_opengl.h> // seems not needed

#include <SDL2/SDL.h>

static struct {
	SDL_Window *window;
	const char *name;
	int width;
	int height;
} w;

void window_init(const char *window_name) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	w.name = window_name ? window_name : "Icraft";
	w.width = 2000;
	w.height = 1000;
	w.window = SDL_CreateWindow(w.name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			w.width, w.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (w.window == NULL) {
		SDL_Quit();
		loge("Fail to create window");
	}

	SDL_GL_CreateContext(w.window);
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		loge("Fail to load OpenGL function pointers");
	}

	SDL_SetWindowGrab(w.window, true);
	SDL_SetRelativeMouseMode(true);

	SDL_SetWindowFullscreen(w.window, SDL_WINDOW_FULLSCREEN);
}

SDL_Window *window_getWindow() { return w.window; }
int window_getWidth() { return w.width; }
int window_getHeight() { return w.height; }

void window_setSize(int width, int height) {
	if (width != 0) {
		w.width = width;
	}
	if (height != 0) {
		w.height = height;
	}
	glViewport(0, 0, w.width, w.height);
}
