#ifndef _ICRAFT_render_ui_scene_h
#define _ICRAFT_render_ui_scene_h

#include <SDL2/SDL_events.h>
#include "element.h"
typedef struct Scene Scene;

Scene *newScene(uiElement *root);
void scene_free(Scene *scene);
void scene_update(Scene *scene, SDL_Event *event);
void scene_render(Scene *scene);

#endif
