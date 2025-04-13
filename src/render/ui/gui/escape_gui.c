#include "../element.h"
#include "../scene.h"

Scene *gui_ofEscape() {
	uiElement *root = ui_newElementLinear((uiElement*[]){
		ui_newElementButton("Back to game", NULL),
		ui_newElementLinear((uiElement*[]){
			ui_newElementButton("Options", NULL),
			ui_newElementLabel("Statistics"),
		}, 2, true),
		ui_newElementButton("Exit", NULL),
	}, 3, false);
	return newScene(root);
}
