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
		ui_newElementButton("回到游戏", backtogame),
		ui_newElementLinear((uiElement*[]){
			ui_newElementButton("选项", NULL),
			ui_newElementLabel(u8"统计"),
		}, 2, true),
		ui_newElementButton("退出游戏", exitgame),
	}, 3, false);
	return newScene("Escape GUI", root, (Keymap[]) {
		{ Action_KEYDOWN, { "Escape" }, backtogame}
	}, 1);
}
