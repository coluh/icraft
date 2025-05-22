#include <stdlib.h>
#include "../../game.h"
#include "element.h"
#include "../scene.h"
#include "../scenemanager.h"
#include "../../render/window.h"
#include "../../render/render_2d.h"

extern Game g;

static void backtogame() {
	// sceneManager_switchTo("InGame GUI");
	sceneManager_pop();
}

static void exitgame() {
	g.running = false;
}

static void on_scene_enter(Scene *self) {
	window_focus(false);
}

static void on_scene_exit(Scene *self) {
	window_focus(true);
}

static void render(Scene *self) {
	twod_setColor(0, 0, 0, 0.5f);
	twod_drawQuad(0, 0, self->w, self->h);
	ui_renderElement(self->root);
}

Scene *gui_ofEscape() {
	Scene *s = newScene("Escape GUI", Scene_GUI, (Keymap[]) {
		{ Action_KEYDOWN, { "Escape" }, backtogame}
	}, 1);
	s->root = ui_newElementLinear((uiElement*[]){
		ui_newElementLabel("游戏菜单"),
		ui_newElementButton("回到游戏", backtogame),
		ui_newElementLinear((uiElement*[]){
			ui_newElementButton("选项", NULL),
			ui_newElementButton("统计", NULL),
		}, 2, true),
		ui_newElementButton("退出游戏", exitgame),
	}, 4, false);
	s->block_event = true;
	s->on_enter = on_scene_enter;
	s->on_exit = on_scene_exit;
	s->render = render;
	return s;
}
