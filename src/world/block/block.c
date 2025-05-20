#include "block.h"
#include <string.h>
#include "../../util/log.h"

#define BLOCK_TYPE_COUNT 256

static BlockType block_types[BLOCK_TYPE_COUNT];

// load all blocks
void block_init() {
	block_types[BLOCK_Air] = (BlockType){ "air", { 0, 0, 0, 0, 0, 0 }, NULL, 0.0f };
	block_types[BLOCK_GrassBlock] = (BlockType){ "grass_block", { 2, 2, 3, 1, 2, 2 }, NULL, 0.9f, ITEM_Dirt };
	block_types[BLOCK_Dirt] = (BlockType){ "dirt", { 3, 3, 3, 3, 3, 3 }, NULL, 0.75f, ITEM_Dirt };
	block_types[BLOCK_Stone] = (BlockType){ "stone", { 4, 4, 4, 4, 4, 4 }, NULL, 7.5f, ITEM_CobbleStone };
	block_types[BLOCK_CobbleStone] = (BlockType) { "cobblestone", { 5, 5, 5, 5, 5, 5 }, NULL, 10.0f, ITEM_CobbleStone };
	// now break item is hardcoded, change it later
}

const BlockType *block_get(BlockID id) {
	return &block_types[id];
}

BlockID block_idOf(const char *name) {
	for (int i = 0; i < BLOCK_Unknown; i++) {
		if (block_types[i].name && (strcmp(block_types[i].name, name) == 0)) {
			return (BlockID)i;
		}
	}
	loge("no such block");
	return BLOCK_Unknown;
}

BlockID block_ofItem(ItemID item) {
	Assert(item_isBlock(item), "item is not block");
	switch (item) {
	case ITEM_GrassBlock:
		return BLOCK_GrassBlock;
	case ITEM_Dirt:
		return BLOCK_Dirt;
	case ITEM_Stone:
		return BLOCK_Stone;
	case ITEM_CobbleStone:
		return BLOCK_CobbleStone;
	default:
		break;
	}
	return BLOCK_Unknown;
}
