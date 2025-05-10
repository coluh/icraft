#include <SDL2/SDL_events.h>
#include "../scene.h"
#include "../scenemanager.h"
#include "gui.h"
#include "../../game.h"
#include "../../player/player.h"

extern Game g;

static void on_scene_enter(Scene *self) {
	setDefaultLayout(self);
}

// void *a, SDL_Event *b
static void pause() {
	sceneManager_push("Escape GUI");
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

static void forward(SDL_Event *ev) {
	g.player->input.forward = 1;
}
static void backward(SDL_Event *ev) {
	g.player->input.backward = 1;
}
static void left(SDL_Event *ev) {
	g.player->input.left = 1;
}
static void right(SDL_Event *ev) {
	g.player->input.right = 1;
}
static void up(SDL_Event *ev) {
	g.player->input.jump = true;
}
static void down(SDL_Event *ev) {
}
static void rotate(SDL_Event *ev) {
	player_rotate(g.player, ev);
}

Scene *gui_ofInGame() {
	Scene *s = newScene("InGame GUI", Scene_GUI, (Keymap[]) {
			{ Action_KEYDOWN, { "Escape" }, pause },
			{ Action_KEYDOWN, { "F3" }, toggleDebugInfor },
			{ Action_KEYPRESSED, { "W" }, forward },
			{ Action_KEYPRESSED, { "S" }, backward },
			{ Action_KEYPRESSED, { "A" }, left },
			{ Action_KEYPRESSED, { "D" }, right },
			{ Action_KEYPRESSED, { "Space" }, up },
			{ Action_KEYPRESSED, { "Left Shift" }, down },
			{ Action_MOUSEMOTION, { 0 }, rotate },
			}, 9);
	s->root = ui_newElementLinear(NULL, 0, false);
	s->on_enter = on_scene_enter;
	return s;
}
