#ifndef _ICRAFT_input_keymap_h
#define _ICRAFT_input_keymap_h

#include <SDL2/SDL_events.h>

typedef enum ActionType {

	// handle
	Action_KEYDOWN, Action_KEYUP,
	Action_MOUSEDOWN, Action_MOUSEUP, Action_MOUSEMOTION, Action_MOUSEWHEEL,

	// update
	Action_KEYPRESSED, Action_MOUSEPRESSED,

} ActionType;

typedef enum MouseName {
	// keep it the same as SDL2's
	Mouse_LEFT = 1, Mouse_MIDDLE, Mouse_RIGHT
} MouseName;

typedef struct Keymap {
	ActionType type; // SDL_Event->type
	union {
		const char *key;
		MouseName button;
	};
	void (*callback)(SDL_Event *ev);
} Keymap;



#endif
