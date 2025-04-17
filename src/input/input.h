#ifndef _ICRAFT_input_input_h
#define _ICRAFT_input_input_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <stdbool.h>

typedef enum ActionType {
	Action_KEYDOWN, Action_KEYUP, Action_KEYPRESSED,
	Action_MOUSEDOWN, Action_MOUSEUP, Action_MOUSEMOTION,
} ActionType;

typedef enum MouseName {
	Mouse_LEFT, Mouse_MIDDLE, Mouse_RIGHT
} MouseName;

typedef struct Keymap {
	ActionType type; // SDL_Event->type
	union {
		const char *key;
		MouseName button;
	};
	void (*callback)(void *target, SDL_Event *ev);
} Keymap;

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
