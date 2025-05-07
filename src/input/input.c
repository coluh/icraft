#include "input.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>

#define MAX_CALLBACK_COUNT 64

void input_handle(SDL_Event *ev) {

	// contain code that moves cursor causing SDL_MOUSEMOTION
	// sceneManager_handle(ev);
}

void input_update(){

	// sceneManager_update();
}
