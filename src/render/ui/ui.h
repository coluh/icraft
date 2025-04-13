#ifndef _ICRAFT_render_ui_ui_h
#define _ICRAFT_render_ui_ui_h

#include <SDL2/SDL_events.h>
#include "scene.h"

void ui_init();
void ui_handle(SDL_Event *event);
void ui_render();
void ui_pushScene(Scene *scene);
Scene *ui_popScene();

#endif
