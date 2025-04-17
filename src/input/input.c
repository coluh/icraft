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

InputCallback callbacks[MAX_CALLBACK_COUNT];
int callbacks_i = 0;

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

Scene *gui_ofEscape();

void input_handle(SDL_Event *ev) {

	sceneManager_handle(ev);

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

	const char *name;

	switch (ev->type) {
	case SDL_WINDOWEVENT:
		if (ev->window.event == SDL_WINDOWEVENT_RESIZED) {
			window_setSize(ev->window.data1, ev->window.data2);
		}
		break;
	case SDL_KEYDOWN:
		switch (ev->key.keysym.sym) {
		case SDLK_ESCAPE:
			// debug...
			name = sceneManager_peekName();
			if (name != NULL && strcmp(name, "Escape GUI") == EQUAL) {
				sceneManager_pop();
				window_focus(true);
				input_setTargetEnabled(callbacks[0].target, true);
			} else {
				sceneManager_push(gui_ofEscape());
				input_setTargetEnabled(callbacks[0].target, false);
				window_focus(false);
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void input_update(){

	// } rd[8] = {
	// 	{"W", rdcam_throttle_high},
	// 	{"S", rdcam_throttle_low},
	// 	{"A", rdcam_yaw_left},
	// 	{"D", rdcam_yaw_right},
	// 	{"J", rdcam_pitch_backward},
	// 	{"K", rdcam_pitch_forward},
	// 	{"H", rdcam_roll_left},
	// 	{"L", rdcam_roll_right},
	// };

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
