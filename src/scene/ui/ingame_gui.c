#include <SDL2/SDL_events.h>
#include "../scene.h"
#include "../../input/input.h"
#include "../scenemanager.h"
#include "../../render/window.h"
#include "../../player/player.h"
#include "gui.h"

// void *a, SDL_Event *b
static void pause() {
	sceneManager_switchTo("Escape GUI");
}

static void toggleDebugInfor() {
	static int show = 0;
	if (!show) {
		sceneManager_push("Main Debug Information");
		show = 1;
	} else {
		sceneManager_pop();
		show = 0;
	}
}

static void on_scene_enter(Scene *self) {
	window_focus(true);
	input_setTargetEnabled(getPlayer(), true);
	setDefaultLayout(self);
}

static void on_scene_exit(Scene *self) {
	window_focus(false);
	input_setTargetEnabled(getPlayer(), false);
}

Scene *gui_ofInGame() {
	Scene *s = newScene("InGame GUI", Scene_GUI, (Keymap[]) {
			{ Action_KEYDOWN, { "Escape" }, pause },
			{ Action_KEYDOWN, { "F3" }, toggleDebugInfor },
			},2);
	s->root = ui_newElementLinear(NULL, 0, false);
	s->on_enter = on_scene_enter;
	s->on_exit = on_scene_exit;
	return s;
}
