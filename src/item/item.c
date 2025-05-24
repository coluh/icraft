#include "item.h"
#include "../util/props.h"
#include "../game.h"
#include "../render/resource.h"
#include "../util/log.h"

extern Game g;

const ItemID blocks[] = {
	ITEM_Unknown,
	ITEM_Dirt, ITEM_CobbleStone, ITEM_GrassBlock, ITEM_Stone,
};

static bool inArray(ItemID id, const ItemID ids[], int len) {
	for (int i = 0; i < len; i++) {
		if (id == ids[i]) {
			return true;
		}
	}
	return false;
}

bool item_isBlock(ItemID id) {
	return inArray(id, blocks, ARRLEN(blocks));
}

unsigned int item_blockTexture(ItemID id) {
	if (item_isBlock(id)) {
		return g.res->textures.block_icons[id].texture;
	}
	logw("No corresponding block texture for itemid: %d", id);
	return g.res->textures.block_icons[ITEM_Unknown].texture;
}
