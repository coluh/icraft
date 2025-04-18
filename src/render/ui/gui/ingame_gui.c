#include <SDL2/SDL_events.h>
#include "../scene.h"
#include "../../../input/input.h"

#include "../scenemanager.h"
#include "../../window.h"
#include "../../../player/player.h"

// void *a, SDL_Event *b
static void pause() {
	sceneManager_switchTo("Escape GUI");
	window_focus(false);
	input_setTargetEnabled(getPlayer(), false);
}

Scene *gui_ofInGame() {
	return newScene(
		"InGame GUI",
		ui_newElementLinear(NULL, 0, false),
		(Keymap[]) {
			{ Action_KEYDOWN, { "Escape" }, pause },
		},
		1
	);
}
