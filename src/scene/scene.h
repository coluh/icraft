#ifndef _ICRAFT_render_ui_scene_h
#define _ICRAFT_render_ui_scene_h

#include <SDL2/SDL_events.h>
#include "ui/element.h"
#include "../input/keymap.h"

typedef enum SceneType {
	Scene_GUI, Scene_HUD, Scene_TEXTS, Scene_CUSTOM,
} SceneType;

// A scene represents one 2D scene, which
// could be UI, HUD, or TEXTS. They can overlap.
typedef struct Scene {
	const char *name;
	SceneType type;
	int x, y, w, h;

	union {
		uiElement *root;
		void *data;
	};

	void (*on_enter)(struct Scene *self);
	void (*on_size_changed)(struct Scene *self, int width, int height);
	void (*update)(struct Scene *self);
	void (*render)(struct Scene *self);
	void (*on_exit)(struct Scene *self);

	int keymaps_count;
	Keymap keymaps[];
} Scene;

Scene *newScene(const char *name, SceneType type, Keymap keymaps[], int count);
void scene_free(Scene *scene);
const char *scene_getName(Scene *scene);

void scene_updateLayout(Scene *scene);

void scene_handle(Scene *scene, SDL_Event *event);
void scene_update(Scene *scene);
void scene_render(Scene *scene);

#endif
