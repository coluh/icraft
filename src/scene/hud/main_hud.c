#include <SDL2/SDL_events.h>
#include <math.h>
#include "../scene.h"
#include "../scenemanager.h"
#include "../../game.h"
#include "../../entity/entity.h"
#include "../../render/window.h"
#include "../../entity/player.h"
#include "../../util/mem.h"
#include "../../render/render_2d.h"
#include "../../world/block/extra.h"
#include "../../world/world.h"

extern Game g;

typedef struct {
	float height_unit;
} MainHUDData;

static void on_size_changed(Scene *self, int width, int height) {
	((MainHUDData*)self->data)->height_unit = ceilf((float)height / 256) * 16;
}

static void on_scene_enter(Scene *self) {
	on_size_changed(self, g.window->width, g.window->height);
}

static void render(Scene *self) {
	const int a = ((MainHUDData*)self->data)->height_unit;
	int w = 10 * a;
	const int t = a / 8;

	int y = g.window->height - a;
	int x = (g.window->width - w) / 2;
	twod_setColor(0, 0, 0, 0.5);
	twod_drawQuad(x-1, y-1, w+2, a+2);
	for (int i = 0; i < 10; i++) {
		x = (g.window->width - w) / 2 + i * a;
		twod_setColor(0.7, 0.7, 0.7, 1.0);
		twod_drawQuad(x, y, a, t);
		twod_drawQuad(x, y+a-t, a, t);
		twod_drawQuad(x+1, y, t-1, a);
		twod_drawQuad(x+a-t, y, t, a);
		// twod_setColor(0.2, 0.2, 0.2, 1.0);
		// twod_drawQuad(x+t, y+t, a-2*t, a-2*t);
	}

	w = g.window->width;
	int h = g.window->height;
	int l = a / 4;
	twod_setColor(1, 1, 1, 1);
	twod_drawQuad(w/2-l/2, h/2-1, l, 2);
	twod_drawQuad(w/2-1, h/2-l/2, 2, l);
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
	g.player->player.input.forward = 1;
}
static void backward(SDL_Event *ev) {
	g.player->player.input.backward = 1;
}
static void left(SDL_Event *ev) {
	g.player->player.input.left = 1;
}
static void right(SDL_Event *ev) {
	g.player->player.input.right = 1;
}
static void up(SDL_Event *ev) {
	g.player->player.input.jump = true;
}
static void down(SDL_Event *ev) {
}
static void rotate(SDL_Event *ev) {
	player_rotate(g.player, ev);
}
static void destroy(SDL_Event *ev) {
	float fx = g.player->player.facing_block.x;
	float fy = g.player->player.facing_block.y;
	float fz = g.player->player.facing_block.z;
	if (world_block(g.world, fx, fy, fz) == BLOCK_Air) {
		return;
	}
	block_destroyCallback(g.world, g.player->player.facing_block.x, g.player->player.facing_block.y, g.player->player.facing_block.z);
	// block_destroy(g.world, g.player->player.facing_block.x, g.player->player.facing_block.y, g.player->player.facing_block.z);
}

Scene *hud_ofMain() {
	Scene *s = newScene("Main HUD", Scene_HUD, (Keymap[]) {
			{ Action_KEYDOWN, { "Escape" }, pause },
			{ Action_KEYDOWN, { "F3" }, toggleDebugInfor },
			{ Action_KEYPRESSED, { "W" }, forward },
			{ Action_KEYPRESSED, { "S" }, backward },
			{ Action_KEYPRESSED, { "A" }, left },
			{ Action_KEYPRESSED, { "D" }, right },
			{ Action_KEYPRESSED, { "Space" }, up },
			{ Action_KEYPRESSED, { "Left Shift" }, down },
			{ Action_MOUSEMOTION, { 0 }, rotate },
			{ Action_MOUSEDOWN, { .button = Mouse_LEFT }, destroy},
			}, 10);
	s->data = zalloc(1, sizeof(MainHUDData)); // store the hud height unit
	s->on_enter = on_scene_enter;
	s->on_size_changed = on_size_changed;
	s->render = render;
	return s;
}
