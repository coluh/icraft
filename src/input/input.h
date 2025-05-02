#ifndef _ICRAFT_input_input_h
#define _ICRAFT_input_input_h

#include "keymap.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

#define DEFAULT_PLAYER_KEYMAPS (Keymap[]) { \
	{ Action_KEYPRESSED, { "W" }, player_forward }, \
	{ Action_KEYPRESSED, { "S" }, player_backward }, \
	{ Action_KEYPRESSED, { "A" }, player_left }, \
	{ Action_KEYPRESSED, { "D" }, player_right }, \
	{ Action_KEYPRESSED, { "Space" }, player_up }, \
	{ Action_KEYPRESSED, { "Left Shift" }, player_down }, \
	{ Action_MOUSEMOTION, { 0 }, player_rotate }, \
}

void input_init();

void input_setCallbacks(void *target, Keymap keymaps[], int count);
void input_setTargetEnabled(void *target, bool enable);

void input_handle(SDL_Event *event);
void input_update();

#endif
