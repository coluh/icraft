#include "input.h"
#include "../render/window.h"
#include "server.h"
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
		if (camera->type == CameraType_FPS) {
			FPSCamera.rotate(camera, dx, dy);
		}
	}
}

void input_update(Camera *camera) {

	struct {
		const char *key;
		void (*handler)(Camera *camera);
	} fps[6] = {
		{"W", FPSCamera.forward},
		{"S", FPSCamera.backward},
		{"A", FPSCamera.left},
		{"D", FPSCamera.right},
		{"Space", FPSCamera.up},
		{"Left Shift", FPSCamera.down},
	};
	struct {
		const char *key;
		void (*handler)(Camera *camera, float ratio);
	} rd[8] = {
		{"W", RDCamera.throttle_high},
		{"S", RDCamera.throttle_low},
		{"A", RDCamera.yaw_left},
		{"D", RDCamera.yaw_right},
		{"J", RDCamera.pitch_backward},
		{"K", RDCamera.pitch_forward},
		{"H", RDCamera.roll_left},
		{"L", RDCamera.roll_right},
	};

	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (camera->type == CameraType_FPS) {
		for (int i = 0; i < 6; i++) {
			if (state[SDL_GetScancodeFromName(fps[i].key)]) {
				fps[i].handler(camera);
			}
		}
	} else if (camera->type == CameraType_RD) {
		for (int i = 0; i < 8; i++) {
			if (state[SDL_GetScancodeFromName(rd[i].key)]) {
				rd[i].handler(camera, 0.6f);
			}
		}
	}

	if (camera->type == CameraType_RD) {
		const float *wsadjkhl = server_receive();
		for (int i = 0; i < 8; i++) {
			if (wsadjkhl[i] != 0.0f) {
				float ratio = wsadjkhl[i];
				if (i >= 2) { ratio = ratio * ratio; }
				rd[i].handler(camera, ratio); // reduce effect of minor operation
			}
		}
	}
}
