#include "item.h"
#include "../util/props.h"

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
