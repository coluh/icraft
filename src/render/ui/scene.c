#include "scene.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_video.h>
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

static void defaultLayout(Scene *s) {
	s->w = window_getWidth();
	s->h = window_getHeight();
	s->root->rect.x = s->w/6;
	s->root->rect.y = s->h/6;
	s->root->rect.w = s->w*2/3;
	s->root->rect.h = 0;
}

Scene *newScene(const char *name, uiElement *root, Keymap keymaps[], int count) {
	Scene *s = zalloc(1, sizeof(Scene) + count * sizeof(Keymap));
	s->name = name;
	s->root = root;
	s->x = 0;
	s->y = 0;
	defaultLayout(s);
	ui_arrangeLinearLayout(root, true);

	s->keymaps_count = count;
	FORR(count) s->keymaps[i] = keymaps[i];
	// UI Keymap can only handle event, not able to update now

	return s;
}

const char *scene_getName(Scene *scene) { return scene->name; }

void scene_free(Scene *scene) {}

void scene_update(Scene *s, SDL_Event *ev) {

	if (ev->type == SDL_WINDOWEVENT && ev->window.event == SDL_WINDOWEVENT_RESIZED) {
		defaultLayout(s);
		ui_arrangeLinearLayout(s->root, true);
	}

	ui_updateElement(s->root, ev);
	FORR(s->keymaps_count) {
		Keymap *km = &s->keymaps[i];
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
