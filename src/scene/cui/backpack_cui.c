#include <SDL2/SDL_mouse.h>
#include "../scene.h"
#include "../../util/mem.h"
#include "../../game.h"
#include "../../render/window.h"
#include "component.h"
#include "../scenemanager.h"
#include "../../entity/entity.h"
#include "../../util/props.h"

extern Game g;

typedef struct BackpackCUIState {

	Slot craft[2][2];
	Slot craft_out;

	Slot picking;
	int splits[64]; // well, how to expand
	bool splitting;
} BackpackCUIState;

static void get_params(int *outer_w, int *outer_h, int *slot_a, int *padding) {
	*outer_w = g.zoom_level*170;
	*slot_a = *outer_w / 9.4f;
	*padding = *slot_a * 0.4f / 2;
	*outer_h = 8**slot_a + 4**padding;
}

// id is from up to bottom, from left to right
// 0
// 1     5 6
// 2     7 8  9
// 3  4
// 10 ~ 18
// 19 ~ 27
// 28 ~ 36
// 37 ~ 45
static void slot_pos(int id, int *x, int *y) {
	int outer_w = g.zoom_level*170;
	int slot_a = outer_w / 9.4f;
	int padding = slot_a * 0.4f / 2;
	int outer_h = 8*slot_a + 4*padding;
	int window_w = g.window->width;
	int window_h = g.window->height;
	int dark_w = 3*slot_a - padding;
	int bx = window_w/2 - outer_w/2 + padding;
	int by = window_h/2 - outer_h/2 + padding;
	if (id >= 0 && id < 4) {
		*x = bx;
		*y = by + id*slot_a;
	} else if (id == 4) {
		*x = bx + slot_a + dark_w;
		*y = by + 3*slot_a;
	} else if (id >= 5 && id < 9) {
		id -= 5;
		*x = bx + slot_a + dark_w + slot_a + padding + id%2 * slot_a;
		*y = by + 0.5*slot_a + (int)(id/2) * slot_a;
	} else if (id == 9) {
		*x = bx + slot_a + dark_w + slot_a + padding + 3*slot_a + padding/2;
		*y = by + 0.5*slot_a + 0.5*slot_a;
	} else if (id >= 10 && id < 37) {
		id -= 10;
		*x = bx + id%9 * slot_a;
		*y = by + 4*slot_a + padding + id/9 * slot_a;
	} else if (id >= 37 && id < 46) {
		id -= 37;
		*x = bx + id%9 * slot_a;
		*y = by + 7*slot_a + 2*padding;
	} else {
		logw("no such id: %d", id);
	}
}

static int slotidof(int mouse_x, int mouse_y) {
	int x, y;
	int outer_w, outer_h, slot_a, padding;
	get_params(&outer_w, &outer_h, &slot_a, &padding);
	for (int i = 0; i < 46; i++) {
		slot_pos(i, &x, &y);
		if (IN_SQUARE(mouse_x, mouse_y, x, y, slot_a)) {
			return i;
		}
	}
	return -1;
}

static Slot *idslot(int id, BackpackCUIState *s) {
	PlayerData *player = &entity_get(g.entities, g.player_ref)->player;
	if (id >= 0 && id < 4) {
		return &player->inventory.armor[id];
	} else if (id == 4) {
		return &player->inventory.offhand;
	} else if (id >= 5 && id < 9) {
		return &s->craft[(id-5)/2][(id-5)%2];
	} else if (id == 9) {
		return &s->craft_out;
	} else if (id >= 10 && id < 37) {
		return &player->inventory.storage[(id-10)/9][(id-10)%9];
	} else if (id >= 37 && id < 46) {
		return &player->inventory.hotbar[(id-37)%9];
	}
	return NULL;
}

static void render(Scene *self) {

	int outer_w, outer_h, slot_a, padding;
	get_params(&outer_w, &outer_h, &slot_a, &padding);
	int window_w = g.window->width;
	int window_h = g.window->height;
	twod_setColor(0, 0, 0, 0.5f);
	twod_drawQuad(0, 0, window_w, window_h);
	int x = window_w/2 - outer_w/2;
	int y = window_h/2 - outer_h/2;
	outline(x, y, outer_w, outer_h, g.zoom_level*2);
	grayquad(x, y, outer_w, outer_h, 0.7);

	BackpackCUIState *s = ((Scene*)self)->data;
	for (int i = 0; i < 46; i++) {
		slot_pos(i, &x, &y);
		draw_slot(x, y, slot_a, slot_a, g.zoom_level);
		draw_item(idslot(i, s), x, y, slot_a, g.zoom_level);
	}

	const int dark_w = 3*slot_a - padding;
	const int dark_x = window_w/2 - outer_w/2 + padding + slot_a;
	const int dark_y = window_h/2 - outer_h/2 + padding;
	const int dark_h = 4*slot_a;
	int shadow = g.zoom_level;
	grayquad(dark_x + shadow, dark_y + shadow, dark_w - 2*shadow, dark_h - 2 * shadow, 0);

	int id, mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	id = slotidof(mouse_x, mouse_y);
	if (id != -1) {
		slot_pos(id, &x, &y);
		twod_setColor(1.0f, 1.0f, 1.0f, 0.5f);
		twod_drawQuad(x + shadow, y + shadow, slot_a - 2*shadow, slot_a - 2*shadow);
	}
	if (s->splitting == true) {
		for (int i = 0; s->splits[i] != -1; i++) {
			slot_pos(s->splits[i], &x, &y);
			twod_setColor(1.0f, 1.0f, 1.0f, 0.5f);
			twod_drawQuad(x + shadow, y + shadow, slot_a - 2*shadow, slot_a - 2*shadow);
		}
	}
	if (s->picking.count > 0) {
		draw_item(&s->picking, mouse_x - slot_a/2, mouse_y - slot_a/2, slot_a, g.zoom_level);
	}

	// player model
}

static void addto_splits(int id, int splits[]) {
	int i = 0;
	// WARNING: not extendable
	while (splits[i] != -1) {
		i++;
	}
	splits[i] = id;
}

static void leftdown(SDL_Event *ev, void *scene) {
	int id = slotidof(ev->button.x, ev->button.y);
	if (id == -1) {
		return;
	}

	BackpackCUIState *s = ((Scene*)scene)->data;
	Slot *slot = idslot(id, s);
	if (s->picking.count == 0) {
		// pick up all
		if (slot->count > 0) {
			s->picking.item = slot->item;
			s->picking.count = slot->count;
			slot->count -= s->picking.count;
		}
	} else {
		// start splitting
		s->splitting = true;
		for (int i = 0; i < 64; i++) {
			s->splits[i] = -1;
		}
		addto_splits(id, s->splits);
	}
}

static bool in_splits(int id, int splits[]) {
	for (int i = 0; i < 64; i++) {
		if (splits[i] == -1) {
			return false;
		} else if (splits[i] == id) {
			return true;
		}
	}
	return false;
}

static int splits_len(int splits[]) {
	int n = 0;
	while (splits[n] != -1) {
		n++;
	}
	return n;
}

static void leftpressed(SDL_Event *none, void *scene) {
	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	int id = slotidof(mouse_x, mouse_y);
	if (id == -1) {
		return;
	}

	BackpackCUIState *s = ((Scene*)scene)->data;
	Slot *slot = idslot(id, s);
	if (s->picking.count > 0 && s->splitting) {
		// TODO: item should be completely the same, not just id
		if (slot->count == 0 || slot->item.id == s->picking.item.id) {
			if (!in_splits(id, s->splits) && id != 9) {
				addto_splits(id, s->splits);
				// int n = splits_len(s->splits);
				// int a = s->picking.count / n;
				// int r = s->picking.count - a * n;
			}
		}
	}
}

static void leftup(SDL_Event *ev, void *scene) {
	int id = slotidof(ev->button.x, ev->button.y);
	if (id == -1) {
		return;
	}

	BackpackCUIState *s = ((Scene*)scene)->data;
	if (s->picking.count > 0 && s->splitting) {
		// put down on average
		int n = splits_len(s->splits);
		int a = s->picking.count/n;
		if (a == 0) {
			a = 1;
		}
		for (int i = 0; s->splits[i] != -1; i++) {
			Slot *slot = idslot(s->splits[i], s);
			if (slot->count == 0) {
				slot->item = s->picking.item;
			}
			if (s->picking.count >= a) {
				s->picking.count -= a;
				slot->count += a;
			} else {
				break;
			}
		}
		// end splits
		s->splitting = false;
	}
}

static void middledown(SDL_Event *ev, void *scene) {
	// not used in survival mode
}

static void middleup(SDL_Event *ev, void *scene) {
	// not used in survival mode
}

static void rightdown(SDL_Event *ev, void *scene) {
	int id = slotidof(ev->button.x, ev->button.y);
	if (id == -1) {
		return;
	}

	BackpackCUIState *s = ((Scene*)scene)->data;
	Slot *slot = idslot(id, s);
	if (s->picking.count == 0) {
		// pick up half
		if (slot->count > 0) {
			s->picking.item = slot->item;
			s->picking.count = (slot->count + 1)/2;
			slot->count -= s->picking.count;
		}
	} else {
		// start splitting
		s->splitting = true;
		for (int i = 0; i < 64; i++) {
			s->splits[i] = -1;
		}
		addto_splits(id, s->splits);
	}
}

static void rightpressed(SDL_Event *none, void *scene) {
	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	int id = slotidof(mouse_x, mouse_y);
	if (id == -1) {
		return;
	}

	BackpackCUIState *s = ((Scene*)scene)->data;
	Slot *slot = idslot(id, s);
	if (s->picking.count > 0 && s->splitting) {
		// TODO: item should be completely the same, not just id
		if (slot->count == 0 || slot->item.id == s->picking.item.id) {
			if (!in_splits(id, s->splits) && id != 9) {
				addto_splits(id, s->splits);
			}
		}
	}
}

static void rightup(SDL_Event *ev, void *scene) {
	int id = slotidof(ev->button.x, ev->button.y);
	if (id == -1) {
		return;
	}

	BackpackCUIState *s = ((Scene*)scene)->data;
	if (s->picking.count > 0 && s->splitting) {
		// put down one by one
		for (int i = 0; s->splits[i] != -1; i++) {
			Slot *slot = idslot(s->splits[i], s);
			if (slot->count == 0) {
				slot->item = s->picking.item;
			}
			if (s->picking.count > 0) {
				s->picking.count--;
				slot->count++;
			} else {
				break;
			}
		}
		s->splitting = false;
	}
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
			{ Action_MOUSEPRESSED, { .button = Mouse_LEFT }, leftpressed },
			{ Action_MOUSEUP, { .button = Mouse_LEFT }, leftup },
			{ Action_MOUSEDOWN, { .button = Mouse_MIDDLE }, middledown },
			{ Action_MOUSEUP, { .button = Mouse_MIDDLE }, middleup },
			{ Action_MOUSEDOWN, { .button = Mouse_RIGHT }, rightdown },
			{ Action_MOUSEPRESSED, { .button = Mouse_RIGHT }, rightpressed },
			{ Action_MOUSEUP, { .button = Mouse_RIGHT }, rightup },
			}, 10);
	s->data = zalloc(1, sizeof(BackpackCUIState));
	s->render = render;
	s->block_event = true;
	s->on_enter = on_scene_enter;
	s->on_exit = on_scene_exit;
	return s;
}
