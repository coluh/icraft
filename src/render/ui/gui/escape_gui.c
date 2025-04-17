#include "../element.h"
#include "../scene.h"
#include "../scenemanager.h"
#include "../../../util/log.h"

void backtogame() {
	sceneManager_pop();
}

void exitgame() {
	logd("emmm exit");
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
	return newScene("Escape GUI", root);
}
