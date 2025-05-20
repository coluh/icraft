#include "di.h"
#include "../../game.h"
#include "../../entity/entity.h"
#include <stdio.h>
#include <string.h>
#include "../scene.h"
#include "../../input/keymap.h"
#include "../../render/render_2d.h"
#include "../../render/font.h"
#include "../../world/world.h"
#include "../../world/block/block.h"
// #include "../../util/log.h"

extern Game g;

static void update(Scene *self) {
	;
}

static void render(Scene *self) {
	const Entity *player = entity_get(g.entities, g.player_ref);
	twod_setColor(0, 0, 0, 0.5);
	twod_drawQuad(0, 0, 100, 48);
	twod_setColor(1, 1, 1, 1);
	char buf[32];
	memset(buf, 0, 32);
	snprintf(buf, 3, "%2d", g.fps);
	font_drawText(buf, 0, 0, 1);
	memset(buf, 0, 32);
	snprintf(buf, 29, "%8.4f, %8.4f, %8.4f", player->position.x, player->position.y, player->position.z);
	font_drawText(buf, 10, 48, 1);
	memset(buf, 0, 32);
	int fx = player->player.facing_block.x;
	int fy = player->player.facing_block.y;
	int fz = player->player.facing_block.z;
	snprintf(buf, 29, "%4d, %4d, %4d", fx, fy, fz);
	font_drawText(buf, 10, 96, 1);
	memset(buf, 0, 32);
	snprintf(buf, 30, "facing: %s", block_get(world_block(g.world, fx, fy, fz))->name);
	font_drawText(buf, 10, 144, 1);
}

Scene *di_ofMain() {
	Scene *s = newScene("Main Debug Information", Scene_TEXTS, (Keymap[]) {
			{ Action_KEYDOWN, {"H"}, NULL},
			}, 1);
	// s->data = zalloc(1, 114);
	s->update = update;
	s->render = render;
	return s;
}
