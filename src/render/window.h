#ifndef _ICRAFT_render_window_h
#define _ICRAFT_render_window_h

#include <SDL2/SDL_video.h>
#include <stdbool.h>

typedef struct Window {
	SDL_Window *window;
	const char *title;
	int width;
	int height;
	bool fullscreen;
} Window;

// contains SDL init, OpenGL Context init
void window_init(const char *window_name);
void window_setSize(int width, int height);
void window_focus(bool focus);
void window_toggleFullscreen();

#endif
