#include "input.h"
#include "../util/props.h"
#include "../render/window.h"
#include "../render/ui/scenemanager.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <string.h>

#define MAX_CALLBACK_COUNT 64

typedef struct InputCallback {
	ActionType type;
	union {
		const char *key;
		MouseName button;
	};
	void (*callback)(void *target, SDL_Event *ev);
	void *target;
	bool disabled;
} InputCallback;

static InputCallback callbacks[MAX_CALLBACK_COUNT];
static int callbacks_i = 0;

void input_init() {}

void input_setCallbacks(void *target, Keymap keymaps[], int count) {
	FORR(count) {
		Keymap *km = &keymaps[i];
		callbacks[callbacks_i].target = target;
		callbacks[callbacks_i].type = km->type;
		callbacks[callbacks_i].key = km->key;
		callbacks[callbacks_i].callback = km->callback;
		callbacks_i++;
	}
}

void input_setTargetEnabled(void *target, bool enable) {
	FORR(callbacks_i) {
		callbacks[i].disabled = !enable;
	}
}

void input_handle(SDL_Event *ev) {

	FORR(callbacks_i) {
		InputCallback *cb = &callbacks[i];
		if (cb->disabled) continue;
		switch (cb->type) {
		case Action_KEYDOWN:
			// if (ev->type == SDL_KEYDOWN && )
			break;
		case Action_KEYUP:
			break;
		case Action_MOUSEDOWN:
			break;
		case Action_MOUSEUP:
			break;
		case Action_MOUSEMOTION:
			if (ev->type == SDL_MOUSEMOTION) {
				cb->callback(cb->target, ev);
			}
			break;
		default:
			break;
		}
	}

	if (ev->type == SDL_WINDOWEVENT && ev->window.event == SDL_WINDOWEVENT_RESIZED) {
		window_setSize(ev->window.data1, ev->window.data2);
	}

	// contain code that moves cursor causing SDL_MOUSEMOTION
	sceneManager_handle(ev);
}

void input_update(){

	const Uint8 *state = SDL_GetKeyboardState(NULL);

	FORR(callbacks_i) {
		InputCallback *cb = &callbacks[i];
		if (cb->disabled) continue;
		switch(cb->type) {
		case Action_KEYPRESSED:
			if (state[SDL_GetScancodeFromName(cb->key)]) {
				cb->callback(cb->target, NULL);
			}
			break;
		default:
			break;
		}
	}
}
