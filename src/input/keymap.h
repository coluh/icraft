#ifndef _ICRAFT_input_keymap_h
#define _ICRAFT_input_keymap_h

#include <SDL2/SDL_events.h>

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



#endif
