#include "scene.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_video.h>
#include <string.h>
#include "ui/element.h"
#include "../util/mem.h"
#include "../util/props.h"


Scene *newScene(const char *name, SceneType type, Keymap keymaps[], int count) {
	Scene *s = zalloc(1, sizeof(Scene) + count * sizeof(Keymap));
	s->name = name;
	s->type = type;

	s->keymaps_count = count;
	FORR(count) s->keymaps[i] = keymaps[i];
	// UI Keymap can only handle event, not able to update now
	// maybe able

	return s;
}

const char *scene_getName(Scene *scene) { return scene->name; }

void scene_free(Scene *scene) {}

void scene_handle(Scene *s, SDL_Event *ev) {

	if (ev->type == SDL_WINDOWEVENT && ev->window.event == SDL_WINDOWEVENT_RESIZED) {
		if (s->on_size_changed) s->on_size_changed(s, ev->window.data1, ev->window.data2);
	}

	switch (s->type) {
	case Scene_GUI:
		ui_updateElement(s->root, ev);
		break;
	case Scene_HUD:
	case Scene_TEXTS:
	case Scene_CUSTOM:
	default:
		break;
	}

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

void scene_update(Scene *s) {
	if (s->update != NULL) {
		s->update(s);
	}

	switch (s->type) {
	case Scene_GUI:
	case Scene_HUD:
	case Scene_TEXTS:
	case Scene_CUSTOM:
	default:
		break;
	}
}

void scene_render(Scene *s) {

	if (s->render != NULL) {
		s->render(s);
		return;
	}

	switch (s->type) {
	case Scene_GUI:
		ui_renderElement(s->root);
		break;
	case Scene_HUD:
	case Scene_TEXTS:
	case Scene_CUSTOM:
	default:
		break;
	}
}
