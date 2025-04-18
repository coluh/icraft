#include <stdlib.h>
#include "../element.h"
#include "../scene.h"
#include "../scenemanager.h"
#include "../../window.h"
#include "../../../input/input.h"
#include "../../../player/player.h"

static void backtogame() {
	sceneManager_switchTo("InGame GUI");
	window_focus(true);
	input_setTargetEnabled(getPlayer(), true);
}

static void exitgame() {
	exit(0);
}

Scene *gui_ofEscape() {
	uiElement *root = ui_newElementLinear((uiElement*[]){
		ui_newElementButton("Back to game", backtogame),
		ui_newElementLinear((uiElement*[]){
			ui_newElementButton("Options", NULL),
			ui_newElementLabel("Statistics"),
		}, 2, true),
		ui_newElementButton("Exit", exitgame),
	}, 3, false);
	return newScene("Escape GUI", root, (Keymap[]) {
		{ Action_KEYDOWN, { "Escape" }, backtogame}
	}, 1);
}
