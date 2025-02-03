#include "input.h"
#include "../render/window.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

void input_handle(SDL_Event *event, Camera *camera) {

	int dx, dy;

	switch (event->type) {
	case SDL_WINDOWEVENT:
		if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
			window_setSize(event->window.data1, event->window.data2);
		}
		break;
	case SDL_KEYDOWN:
		switch (event->key.keysym.sym) {
		default:
			break;
		}
		break;
	case SDL_MOUSEMOTION:
		dx = event->motion.xrel;
		dy = event->motion.yrel;
		FPSCamera.rotate(camera, dx, dy);
	}
}

void input_update(Camera *camera) {

	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_GetScancodeFromName("W")]) {
		FPSCamera.forward(camera);
	}
	if (state[SDL_GetScancodeFromName("S")]) {
		FPSCamera.backward(camera);
	}
	if (state[SDL_GetScancodeFromName("A")]) {
		FPSCamera.left(camera);
	}
	if (state[SDL_GetScancodeFromName("D")]) {
		FPSCamera.right(camera);
	}
	if (state[SDL_GetScancodeFromName("Space")]) {
		FPSCamera.up(camera);
	}
	if (state[SDL_GetScancodeFromName("Left Shift")]) {
		FPSCamera.down(camera);
	}
}
