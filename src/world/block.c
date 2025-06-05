#include "block.h"
#include <stdbool.h>
#include <string.h>
#include "../util/log.h"

#define BLOCK_TYPE_COUNT 256

static BlockType block_types[BLOCK_TYPE_COUNT];

// load all blocks
void block_init() {
	block_types[BLOCK_Air] = (BlockType){ "air", { 0, 0, 0, 0, 0, 0 }, NULL, 0, 0, BLOCK_TRANSPARENT|BLOCK_WATERABLE };
	block_types[BLOCK_GrassBlock] = (BlockType){ "grass_block", { 2, 2, 3, 1, 2, 2 }, NULL, 0.9f, ITEM_Dirt };
	block_types[BLOCK_Dirt] = (BlockType){ "dirt", { 3, 3, 3, 3, 3, 3 }, NULL, 0.75f, ITEM_Dirt };
	block_types[BLOCK_Stone] = (BlockType){ "stone", { 4, 4, 4, 4, 4, 4 }, NULL, 7.5f, ITEM_CobbleStone };
	block_types[BLOCK_CobbleStone] = (BlockType) { "cobblestone", { 5, 5, 5, 5, 5, 5 }, NULL, 10.0f, ITEM_CobbleStone };
	block_types[BLOCK_Poppy] = (BlockType) { "poppy", { 6, 0, 0, 0, 0, 0 }, NULL, 0.0f, ITEM_Poppy, BLOCK_TRANSPARENT|BLOCK_PLANT|BLOCK_WATERABLE };
	block_types[BLOCK_Dandelion] = (BlockType) { "dandelion", { 7, 0, 0, 0, 0, 0 }, NULL, 0.0f, ITEM_Dandelion, BLOCK_TRANSPARENT|BLOCK_PLANT|BLOCK_WATERABLE };
	block_types[BLOCK_OakLog] = (BlockType) { "oak_log", { 13, 13, 12, 12, 13, 13 }, NULL, 3.0f, ITEM_OakLog };
	block_types[BLOCK_OakPlank] = (BlockType) { "oak_planks", { 14, 14, 14, 14, 14, 14 }, NULL, 3.0f, ITEM_OakPlank };
	block_types[BLOCK_OakLeave] = (BlockType) { "oak_leaves", { 15, 15, 15, 15, 15, 15 }, NULL, 0.0f, ITEM_OakLeave, BLOCK_TRANSPARENT|BLOCK_WATERABLE }; // TODO: no item
	block_types[BLOCK_CraftingTable] = (BlockType) { "crafting_table", { 18, 17, 14, 16, 17, 17 }, NULL, 3.0f, ITEM_CraftingTable };

	block_types[BLOCK_Unknown] = (BlockType) { "unknown", { 0, 0, 0, 0, 0, 0 }, NULL, 0.0f, ITEM_Unknown };
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
	switch (item) {
	case ITEM_GrassBlock:
		return BLOCK_GrassBlock;
	case ITEM_Dirt:
		return BLOCK_Dirt;
	case ITEM_Stone:
		return BLOCK_Stone;
	case ITEM_CobbleStone:
		return BLOCK_CobbleStone;
	case ITEM_Poppy:
		return BLOCK_Poppy;
	case ITEM_Dandelion:
		return BLOCK_Dandelion;
	case ITEM_OakLog:
		return BLOCK_OakLog;
	case ITEM_OakPlank:
		return BLOCK_OakPlank;
	case ITEM_OakLeave:
		return BLOCK_OakLeave;
	case ITEM_CraftingTable:
		return BLOCK_CraftingTable;
	case ITEM_Unknown:
		return BLOCK_Unknown;
	default:
		break;
	}
	return BLOCK_Unknown;
}

