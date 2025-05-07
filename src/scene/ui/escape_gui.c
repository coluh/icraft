#include <stdlib.h>
#include "../../game.h"
#include "element.h"
#include "../scene.h"
#include "../scenemanager.h"
#include "../../render/window.h"

extern Game g;

static void backtogame() {
	// sceneManager_switchTo("InGame GUI");
	sceneManager_pop();
}

static void exitgame() {
	g.running = false;
}

void setDefaultLayout(Scene *self) {
	self->x = 0;
	self->y = 0;
	self->w = window_getWidth();
	self->h = window_getHeight();
	self->root->rect.h = self->h*2/3;
	self->root->rect.w = self->root->rect.h*4/3;
	self->root->rect.h /= 4;
	self->root->rect.x = (self->w - self->root->rect.w) / 2;
	self->root->rect.y = (self->h - self->root->rect.h) / 2;
	self->root->rect.y -= self->h/4;
	ui_arrangeLinearLayout(self->root, true);
}

static void on_scene_enter(Scene *self) {
	window_focus(false);
	setDefaultLayout(self);
}

static void on_scene_exit(Scene *self) {
	window_focus(true);
}

Scene *gui_ofEscape() {
	Scene *s = newScene("Escape GUI", Scene_GUI, (Keymap[]) {
		{ Action_KEYDOWN, { "Escape" }, backtogame}
	}, 1);
	s->root = ui_newElementLinear((uiElement*[]){
		ui_newElementButton("回到游戏", backtogame),
		ui_newElementLinear((uiElement*[]){
			ui_newElementButton("选项", NULL),
			ui_newElementLabel(u8"统计"),
		}, 2, true),
		ui_newElementButton("退出游戏", exitgame),
	}, 3, false);
	s->block_event = true;
	s->on_enter = on_scene_enter;
	s->on_exit = on_scene_exit;
	return s;
}
