#ifndef _ICRAFT_render_ui_scenemanager_h
#define _ICRAFT_render_ui_scenemanager_h

#include <SDL2/SDL_events.h>

void sceneManager_init();

void sceneManager_push(const char *name);
void sceneManager_pop();
// pop and push
void sceneManager_switchTo(const char *sceneName);
// see top scene
const char *sceneManager_peek();

void sceneManager_handle(SDL_Event *event);
void sceneManager_update();
void sceneManager_render();

#endif
