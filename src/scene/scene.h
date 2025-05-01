#ifndef _ICRAFT_render_ui_scene_h
#define _ICRAFT_render_ui_scene_h

#include <SDL2/SDL_events.h>
#include "element.h"
#include "../../input/input.h"

typedef enum SceneType {
	Scene_GUI, Scene_HUD, Scene_TEXTS,
} SceneType;

// A scene represents one 2D scene, which
// could be UI, HUD, or TEXTS. They can overlap.
typedef struct Scene Scene;

Scene *newScene(const char *name, uiElement *root, Keymap keymaps[], int count);
void scene_free(Scene *scene);
const char *scene_getName(Scene *scene);

void scene_updateLayout(Scene *scene);

void scene_handle(Scene *scene, SDL_Event *event);
void scene_update(Scene *scene);
void scene_render(Scene *scene);

#endif
