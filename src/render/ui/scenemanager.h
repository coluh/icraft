#ifndef _ICRAFT_render_ui_scenemanager_h
#define _ICRAFT_render_ui_scenemanager_h

#include <SDL2/SDL_events.h>

void sceneManager_init();

void sceneManager_switchTo(const char *sceneName);

void sceneManager_handle(SDL_Event *event);
void sceneManager_render();

const char *sceneManager_getCurrent();

#endif
