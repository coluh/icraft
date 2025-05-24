#include <SDL2/SDL_events.h>
#include <stdio.h>
#include "../scene.h"
#include "../scenemanager.h"
#include "component.h"
#include "../../game.h"
#include "../../entity/entity.h"
#include "../../entity/player.h"
#include "../../entity/bodies.h"
#include "../../world/block/extra.h"
#include "../../world/world.h"
#include "../../render/window.h"
#include "../../render/render_2d.h"
#include "../../render/font.h"
#include "../../util/props.h"
#include "../../physics/collision.h"

extern Game g;

static void render(Scene *self) {
	const int a = g.zoom_level * 16;
	int w = 10 * a;
	const int t = a / 16;

	const PlayerData *player = &entity_get(g.entities, g.player_ref)->player;

	int y = g.window->height - a - t;
	int x = (g.window->width - w) / 2;
	char count[3];
	// big outer border
	twod_setColor(0, 0, 0, 1);
	bar(x-t, y-t, w+2*t, a+2*t, t);
	twod_setColor(0, 0, 0, 0.5);
	twod_drawQuad(x, y, w, a);
	for (int i = 0; i < 10; i++) {
		x = (g.window->width - w) / 2 + i * a;
		const Slot *slot = &player->inventory.hotbar[i];
		if (slot->count > 0) {
			twod_drawTexture(x, y, a, a, item_texture(slot->item.id));
		}
		twod_setColor(0.7, 0.7, 0.7, 1.0);
		bar(x, y, a, a, t);
		twod_setColor(0.5, 0.5, 0.5, 1.0);
		bar(x+t, y+t, a-2*t, a-2*t, t);
	}
	x = (g.window->width - w) / 2;
	for (int i = 0; i < 10; i++) {
		x = (g.window->width - w) / 2 + i * a;
		if (i == player->holding) {
			twod_setColor(0, 0, 0, 1);
			bar(x-2*t, y-2*t, a+4*t, a+4*t, t);
			twod_setColor(0.9, 0.9, 0.9, 1);
			bar(x-t, y-t, a+2*t, a+2*t, 2*t);
		}
		// draw hotbar items
		const Slot *slot = &player->inventory.hotbar[i];
		if (slot->count > 1) {
			if (slot->count < 10) {
				snprintf(count, 2, "%1d", slot->count);
			} else {
				snprintf(count, 3, "%2d", slot->count);
			}
			twod_setColor(0.0f, 0.0f, 0.0f, 1.0f);
			font_drawTextCentered(count, x+a*0.7+2, y+a*0.8+2, LEVEL_CHOOSE(g.zoom_level, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0));
			twod_setColor(1.0f, 1.0f, 1.0f, 1.0f);
			font_drawTextCentered(count, x+a*0.7, y+a*0.8, LEVEL_CHOOSE(g.zoom_level, 0.5, 0.75, 1.0, 1.5, 2.0, 2.5, 3.0));
		}
	}

	// draw '+'
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
	entity_get(g.entities, g.player_ref)->player.input.forward = 1;
}
static void backward(SDL_Event *ev) {
	entity_get(g.entities, g.player_ref)->player.input.backward = 1;
}
static void left(SDL_Event *ev) {
	entity_get(g.entities, g.player_ref)->player.input.left = 1;
}
static void right(SDL_Event *ev) {
	entity_get(g.entities, g.player_ref)->player.input.right = 1;
}
static void up(SDL_Event *ev) {
	entity_get(g.entities, g.player_ref)->player.input.jump = true;
}
static void down(SDL_Event *ev) {
}
static void rotate(SDL_Event *ev) {
	player_rotate(entity_get(g.entities, g.player_ref), ev);
}

static void destroy(SDL_Event *ev) {
	const Entity *player = entity_get(g.entities, g.player_ref);
	float fx = player->player.facing_block.x;
	float fy = player->player.facing_block.y;
	float fz = player->player.facing_block.z;
	if (world_block(g.world, fx, fy, fz) == BLOCK_Air) {
		return;
	}
	block_destroyCallback(g.world, player->player.facing_block.x, player->player.facing_block.y, player->player.facing_block.z);
	// block_destroy(g.world, g.player->player.facing_block.x, g.player->player.facing_block.y, g.player->player.facing_block.z);
}

static void put(SDL_Event *ev) {
	Entity *entity = entity_get(g.entities, g.player_ref);
	PlayerData *player = &entity->player;
	const IV3 *put_pos = &player->putable_block;
	Slot *slot = &player->inventory.hotbar[player->holding];
	if (slot->count > 0) {
		Item *item = &slot->item;
		ItemID id = item->id;
		if (!item_isBlock(id)) {
			// TODO: support more operations
			return;
		}

		BlockID block = block_ofItem(id);
		if (world_block(g.world, put_pos->x, put_pos->y, put_pos->z) == BLOCK_Air) {
			Body *block_body = &(Body){put_pos->x, put_pos->y, put_pos->z, 1.0f, 1.0f, 1.0f};
			Body *player_body = BODYP(entity->position, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH);
			if (collision_overlap(block_body, player_body)) {
				return;
			}
			// item -> block
			slot->count--;
			world_modifyBlock(g.world, put_pos->x, put_pos->y, put_pos->z, block);
		}
	}
}

static void switch_holding(SDL_Event *ev) {
	PlayerData *player = &entity_get(g.entities, g.player_ref)->player;
	if (ev->wheel.y > 0) {
		player->holding--;
		while (player->holding < 0)
			player->holding += 10;
	}
	if (ev->wheel.y < 0) {
		player->holding++;
		while (player->holding >= 10)
			player->holding -= 10;
	}
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
			{ Action_MOUSEPRESSED, { .button = Mouse_LEFT }, destroy},
			{ Action_MOUSEDOWN, { .button = Mouse_RIGHT }, put},
			{ Action_MOUSEWHEEL, { 0 }, switch_holding},
			}, 12);
	s->render = render;
	return s;
}
