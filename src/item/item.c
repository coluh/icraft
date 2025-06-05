#include "item.h"
#include "../util/props.h"
#include "../game.h"
#include "../render/resource.h"
#include "../util/log.h"

extern Game g;

const char *item_name(Item *item) {
	if (item->name) {
		return item->name;
	}

	switch (item->id) {
	case ITEM_Unknown:
		return "unknown";
	case ITEM_Dirt:
		return "dirt";
	case ITEM_CobbleStone:
		return "cobblestone";
	case ITEM_GrassBlock:
		return "grass_block";
	case ITEM_Stone:
		return "stone";
	case ITEM_Poppy:
		return "poppy";
	case ITEM_Dandelion:
		return "dandelion";
	case ITEM_Bucket:
		return "bucket";
	case ITEM_WaterBucket:
		return "water_bucket";
	case ITEM_OakLog:
		return "oak_log";
	case ITEM_OakPlank:
		return "oak_plank";
	case ITEM_OakLeave:
		return "oak_leave";
	case ITEM_CraftingTable:
		return "crafting_table";
	default:
		return "anonymous";
	}

	loge("impossible");
	return NULL;
}

static bool inArray(ItemID id, const ItemID ids[], int len) {
	for (int i = 0; i < len; i++) {
		if (id == ids[i]) {
			return true;
		}
	}
	return false;
}

bool item_is3d(ItemID id) {
	static const ItemID cubes[] = {
		ITEM_Unknown,
		ITEM_Dirt, ITEM_CobbleStone, ITEM_GrassBlock, ITEM_Stone,
		ITEM_OakLog, ITEM_OakPlank, ITEM_OakLeave, ITEM_CraftingTable,
	};
	return inArray(id, cubes, ARRLEN(cubes));
}

unsigned int item_3dIconTexture(ItemID id) {
	if (item_is3d(id)) {
		return g.res->textures.cube_icons[id].texture;
	}
	logw("No corresponding 3d texture for itemid: %d", id);
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
	case ITEM_OakLog:
	case ITEM_OakPlank:
	case ITEM_OakLeave:
	case ITEM_CraftingTable:
		logw("you should not use plane block texture. if you are drawing icon you should use item_cubeIconTexture");
		return 0;
	case ITEM_Poppy:
		return 6;
	case ITEM_Dandelion:
		return 7;
	case ITEM_Bucket:
		return 8;
	case ITEM_WaterBucket:
		return 9;
	}
	return 0;
}

bool item_putable(ItemID id) {
	static const ItemID putables[] = {
		ITEM_Unknown, ITEM_Dirt, ITEM_CobbleStone, ITEM_GrassBlock, ITEM_Stone,
		ITEM_Poppy, ITEM_Dandelion, ITEM_OakLog, ITEM_OakPlank, ITEM_OakLeave,
		ITEM_CraftingTable,
	};
	return inArray(id, putables, ARRLEN(putables));
}
