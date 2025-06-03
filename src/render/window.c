#include "window.h"
#include "gl.h"
#include "../game.h"
#include "../util/log.h"
#include "../util/mem.h"
#include "../util/props.h"
#include <stdbool.h>
#include <glad/glad.h>
/* glad first */
#include <SDL2/SDL.h>

extern Game g;

void window_init(const char *window_name) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	Window *w = zalloc(1, sizeof(Window));
	g.window = w;
	w->title = window_name ? window_name : "Icraft";
	w->width = 1400;
	w->height = 800;
	w->fullscreen = false;
	w->window = SDL_CreateWindow(w->title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			w->width, w->height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (w->window == NULL) {
		SDL_Quit();
		loge("Fail to create window");
	}

	SDL_GL_CreateContext(w->window);
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		loge("Fail to load OpenGL function pointers");
	}

	window_focus(true);
}

void window_setSize(int width, int height) {
	if (width != 0) {
		g.window->width = width;
	}
	if (height != 0) {
		g.window->height = height;
	}
	g.zoom_level = MAX(ceilf((g.window->height - 240) / 240.0f), 1);
	gl_setSize(g.window->width, g.window->height);
	// WARNING: did you forget to update camera aspect?
}

void window_focus(bool focus) {
	if (focus) {
		SDL_SetWindowGrab(g.window->window, true);
		SDL_SetRelativeMouseMode(true);
		SDL_WarpMouseInWindow(g.window->window, g.window->width/2, g.window->height/2);
	} else {
		SDL_SetWindowGrab(g.window->window, false);
		SDL_SetRelativeMouseMode(false);
	}
}

void window_toggleFullscreen() {
	if (!g.window->fullscreen) {
		SDL_SetWindowFullscreen(g.window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		g.window->fullscreen = true;
	} else {
		SDL_SetWindowFullscreen(g.window->window, 0);
		g.window->fullscreen = false;
	}
}
