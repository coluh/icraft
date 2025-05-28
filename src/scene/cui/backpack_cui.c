#include "../scene.h"
#include "../../util/mem.h"
#include "../../game.h"
#include "../../render/window.h"
#include "component.h"
#include "../scenemanager.h"
#include "../../entity/entity.h"

extern Game g;

typedef struct BackpackCUIState {
	;
} BackpackCUIState;

static void render(Scene *self) {
	// BackpackCUIState *s = self->data;

	int window_w = g.window->width;
	int window_h = g.window->height;
	twod_setColor(0, 0, 0, 0.5f);
	twod_drawQuad(0, 0, window_w, window_h);
	int outer_h = g.zoom_level*160;
	int outer_w = g.zoom_level*170;
	int x = window_w/2 - outer_w/2;
	int y = window_h/2 - outer_h/2;
	outline(x, y, outer_w, outer_h, g.zoom_level*2);
	grayquad(x, y, outer_w, outer_h, 0.7);

	const PlayerData *player = &entity_get(g.entities, g.player_ref)->player;

	// slots
	int slot_a = outer_w / 9.4f;
	int padding = slot_a * 0.4f / 2;
	y = y + outer_h - padding;
	x = x + padding;
	y -= slot_a;
	for (int i = 0; i < 9; i++) {
		// hotbar
		slot(x, y, slot_a, slot_a, g.zoom_level);
		slot_item(&player->inventory.hotbar[i], x, y, slot_a, g.zoom_level);
		x += slot_a;
	}
	x -= 9*slot_a;
	y -= padding + 3*slot_a;
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 9; i++) {
			// storage
			slot(x + i*slot_a, y + j*slot_a, slot_a, slot_a, g.zoom_level);
			slot_item(&player->inventory.storage[j][i], x, y, slot_a, g.zoom_level);
		}
	}
	y -= padding + 4*slot_a;
	for (int j = 0; j < 4; j++) {
		// armor
		slot(x, y + j*slot_a, slot_a, slot_a, g.zoom_level);
		slot_item(&player->inventory.armor[j], x, y, slot_a, g.zoom_level);
	}
	x += slot_a;
	const int dark_w = 3*slot_a - padding;
	slot(x, y, dark_w, 4*slot_a, g.zoom_level);
	// const int dark_x = x;
	// const int dark_y = y;
	// const int dark_h = 4*slot_a;
	int shadow = g.zoom_level;
	grayquad(x + shadow, y + shadow, dark_w - 2*shadow, 4*slot_a - 2 * shadow, 0);
	x += dark_w;
	y += 4*slot_a - slot_a;
	// offhand
	slot(x, y, slot_a, slot_a, g.zoom_level);
	slot_item(&player->inventory.offhand, x, y, slot_a, g.zoom_level);
	y += -3*slot_a + 0.5*slot_a;
	x += slot_a + padding;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			slot(x + i*slot_a, y + j*slot_a, slot_a, slot_a, g.zoom_level);
		}
	}
	x += 3*slot_a + padding/2;
	y += 0.5*slot_a;
	slot(x, y, slot_a, slot_a, g.zoom_level);

	// player model
}

static void leftdown(SDL_Event *ev, void *scene) {
	// BackpackCUIState *s = ((Scene*)scene)->data;
}

static void leftup(SDL_Event *ev, void *scene) {
	// BackpackCUIState *s = ((Scene*)scene)->data;
}

static void middledown(SDL_Event *ev, void *scene) {
	// BackpackCUIState *s = ((Scene*)scene)->data;
}

static void middleup(SDL_Event *ev, void *scene) {
	// BackpackCUIState *s = ((Scene*)scene)->data;
}

static void rightdown(SDL_Event *ev, void *scene) {
	// BackpackCUIState *s = ((Scene*)scene)->data;
}

static void rightup(SDL_Event *ev, void *scene) {
	// BackpackCUIState *s = ((Scene*)scene)->data;
}

static void close(SDL_Event *ev, void *scene) {
	sceneManager_pop();
}

static void on_scene_enter(Scene *self) {
	window_focus(false);
}

static void on_scene_exit(Scene *self) {
	window_focus(true);
}

Scene *cui_ofBackpack() {
	Scene *s = newScene("Backpack CUI", Scene_CUI, (Keymap[]) {
			{ Action_KEYDOWN, { "E" }, close},
			{ Action_KEYDOWN, { "Escape" }, close},
			{ Action_MOUSEDOWN, { .button = Mouse_LEFT }, leftdown },
			{ Action_MOUSEUP, { .button = Mouse_LEFT }, leftup },
			{ Action_MOUSEDOWN, { .button = Mouse_MIDDLE }, middledown },
			{ Action_MOUSEUP, { .button = Mouse_MIDDLE }, middleup },
			{ Action_MOUSEDOWN, { .button = Mouse_RIGHT }, rightdown },
			{ Action_MOUSEUP, { .button = Mouse_RIGHT }, rightup },
			}, 8);
	s->data = zalloc(1, sizeof(BackpackCUIState));
	s->render = render;
	s->block_event = true;
	s->on_enter = on_scene_enter;
	s->on_exit = on_scene_exit;
	return s;
}
