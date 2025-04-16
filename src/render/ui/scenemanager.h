#ifndef _ICRAFT_render_ui_scenemanager_h
#define _ICRAFT_render_ui_scenemanager_h

#include <SDL2/SDL_events.h>
#include "scene.h"

void sceneManager_init();
void sceneManager_handle(SDL_Event *event);
void sceneManager_render();
void sceneManager_pushScene(Scene *scene);
Scene *sceneManager_popScene();

#endif
