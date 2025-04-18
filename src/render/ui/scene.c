#include "scene.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <string.h>
#include "element.h"
#include "../../util/mem.h"
#include "../../render/window.h"
#include "../../input/input.h"
#include "../../util/props.h"

typedef struct Scene {
	const char *name;
	int x, y, w, h;
	uiElement *root;

	int keymaps_count;
	Keymap keymaps[];
} Scene;

Scene *newScene(const char *name, uiElement *root, Keymap keymaps[], int count) {
	Scene *s = zalloc(1, sizeof(Scene) + count * sizeof(Keymap));
	s->name = name;
	s->root = root;
	s->x = 0;
	s->y = 0;
	s->w = window_getWidth();
	s->h = window_getHeight();
	root->rect.x = s->w/6;
	root->rect.y = s->h/6;
	root->rect.w = s->w*2/3;
	root->rect.h = s->h*2/3;
	ui_arrangeLinearLayout(root);

	s->keymaps_count = count;
	FORR(count) s->keymaps[i] = keymaps[i];
	// UI Keymap can only handle event, not able to update now

	return s;
}

const char *scene_getName(Scene *scene) { return scene->name; }

void scene_free(Scene *scene) {}

void scene_update(Scene *scene, SDL_Event *ev) {
	ui_updateElement(scene->root, ev);
	FORR(scene->keymaps_count) {
		Keymap *km = &scene->keymaps[i];
		// UI Keymap can only handle event, not able to update now
		switch (km->type) {
		case Action_KEYDOWN:
			if (ev->type == SDL_KEYDOWN && strcmp(km->key, SDL_GetKeyName(ev->key.keysym.sym)) == EQUAL) {
				km->callback(NULL, NULL);
			}
			break;
		default:
			break;
		}
	}
}

void scene_render(Scene *scene) {
	ui_renderElement(scene->root);
}
