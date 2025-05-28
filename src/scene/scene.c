#include "scene.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <string.h>
#include "gui/element.h"
#include "../util/mem.h"
#include "../util/props.h"
#include "../render/window.h"
#include "../util/log.h"

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

void setDefaultLayout(Scene *self) {
	self->x = 0;
	self->y = 0;
	self->w = g.window->width;
	self->h = g.window->height;
	self->root->rect.w = g.zoom_level * 200;
	self->root->rect.h = 0;
	self->root->rect.x = (self->w - self->root->rect.w) / 2;
	self->root->rect.y = self->h/6;
	ui_arrangeLinearLayout(self->root);
}

void scene_handle(Scene *s, SDL_Event *ev) {

	if (ev->type == SDL_WINDOWEVENT && ev->window.event == SDL_WINDOWEVENT_RESIZED) {
		if (s->on_size_changed) s->on_size_changed(s, ev->window.data1, ev->window.data2);
		// NOTE: gui scene root should be linear layout
		if (s->type == Scene_GUI) {
			setDefaultLayout(s);
		}
	}

	switch (s->type) {
	case Scene_GUI:
		ui_updateElement(s->root, ev);
		break;
	case Scene_HUD:
	case Scene_CUI:
	case Scene_TEXTS:
	case Scene_CUSTOM:
	default:
		break;
	}

	FORR(s->keymaps_count) {
		Keymap *km = &s->keymaps[i];
		switch (km->type) {
		case Action_KEYDOWN:
			if (ev->type == SDL_KEYDOWN && strcmp(km->key, SDL_GetKeyName(ev->key.keysym.sym)) == EQUAL) {
				km->callback(NULL, s);
			}
			break;
		case Action_MOUSEDOWN:
			if (ev->type == SDL_MOUSEBUTTONDOWN && ev->button.button == km->button) {
				km->callback(NULL, s);
			}
			break;
		case Action_MOUSEMOTION:
			if (ev->type == SDL_MOUSEMOTION) {
				km->callback(ev, s);
			}
			break;
		case Action_MOUSEWHEEL:
			if (ev->type == SDL_MOUSEWHEEL) {
				km->callback(ev, s);
			}
			break;
		default:
			break;
		}
	}
}

void scene_update(Scene *s, bool input) {
	if (s->update != NULL) {
		s->update(s);
	}

	switch (s->type) {
	case Scene_GUI:
	case Scene_HUD:
	case Scene_CUI:
	case Scene_TEXTS:
	case Scene_CUSTOM:
	default:
		break;
	}

	if (!input) {
		return;
	}

	const Uint8 *state = SDL_GetKeyboardState(NULL);
	Uint32 buttons = SDL_GetMouseState(NULL, NULL);
	FORR(s->keymaps_count) {
		Keymap *km = &s->keymaps[i];
		switch (km->type) {
		case Action_KEYPRESSED:
			if (state[SDL_GetScancodeFromName(km->key)]) {
				km->callback(NULL, s);
			}
			break;
		case Action_MOUSEPRESSED:
			if (((km->button == Mouse_LEFT) && (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))) ||
					((km->button == Mouse_MIDDLE) && (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE))) ||
					((km->button == Mouse_RIGHT) && (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)))) {
				km->callback(NULL, s);
			}
			break;
		default:
			// these are handled in handle()
			break;
		}
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
	case Scene_CUI:
	case Scene_TEXTS:
	case Scene_CUSTOM:
	default:
		break;
	}
}
