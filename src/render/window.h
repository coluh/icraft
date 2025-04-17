#ifndef _ICRAFT_render_window_h
#define _ICRAFT_render_window_h

#include <SDL2/SDL_video.h>
#include <stdbool.h>

void window_init(const char *window_name);
SDL_Window *window_getWindow();
int window_getWidth();
int window_getHeight();
void window_setSize(int width, int height);
void window_focus(bool focus);

#endif
