#include "item.h"
#include "../util/props.h"
#include "../game.h"
#include "../render/resource.h"
#include "../util/log.h"

extern Game g;

static bool inArray(ItemID id, const ItemID ids[], int len) {
	for (int i = 0; i < len; i++) {
		if (id == ids[i]) {
			return true;
		}
	}
	return false;
}

bool item_isCube(ItemID id) {
	const ItemID cubes[] = {
		ITEM_Unknown,
		ITEM_Dirt, ITEM_CobbleStone, ITEM_GrassBlock, ITEM_Stone,
	};
	return inArray(id, cubes, ARRLEN(cubes));
}

unsigned int item_cubeIconTexture(ItemID id) {
	if (item_isCube(id)) {
		return g.res->textures.cube_icons[id].texture;
	}
	logw("No corresponding cube texture for itemid: %d", id);
	return g.res->textures.cube_icons[ITEM_Unknown].texture;
}

int item_textureIndex(ItemID id) {
	switch (id) {
	case ITEM_Unknown:
		return 0;
	case ITEM_Dirt:
	case ITEM_CobbleStone:
	case ITEM_GrassBlock:
	case ITEM_Stone:
		logw("you should not use plane block texture. if you are drawing icon you should use item_cubeIconTexture");
		return 0;
	case ITEM_Poppy:
		return 6;
	case ITEM_Dandelion:
		return 7;
	}
	return 0;
}

bool item_putable(ItemID id) {
	return true;
}
