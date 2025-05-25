#include "block.h"
#include <stdbool.h>
#include <string.h>
#include "../../util/log.h"
#include "../../util/props.h"

#define BLOCK_TYPE_COUNT 256

static BlockType block_types[BLOCK_TYPE_COUNT];

// load all blocks
void block_init() {
	block_types[BLOCK_Air] = (BlockType){ "air", { 0, 0, 0, 0, 0, 0 }, NULL, 0.0f };
	block_types[BLOCK_GrassBlock] = (BlockType){ "grass_block", { 2, 2, 3, 1, 2, 2 }, NULL, 0.9f, ITEM_Dirt };
	block_types[BLOCK_Dirt] = (BlockType){ "dirt", { 3, 3, 3, 3, 3, 3 }, NULL, 0.75f, ITEM_Dirt };
	block_types[BLOCK_Stone] = (BlockType){ "stone", { 4, 4, 4, 4, 4, 4 }, NULL, 7.5f, ITEM_CobbleStone };
	block_types[BLOCK_CobbleStone] = (BlockType) { "cobblestone", { 5, 5, 5, 5, 5, 5 }, NULL, 10.0f, ITEM_CobbleStone };
	block_types[BLOCK_Poppy] = (BlockType) { "poppy", { 6, 0, 0, 0, 0, 0 }, NULL, 0.0f, ITEM_Poppy };
	block_types[BLOCK_Dandelion] = (BlockType) { "dandelion", { 7, 0, 0, 0, 0, 0 }, NULL, 0.0f, ITEM_Dandelion };

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
	case ITEM_Unknown:
		return BLOCK_Unknown;
	default:
		break;
	}
	return BLOCK_Unknown;
}

static bool inArray(BlockID id, const BlockID ids[], int len) {
	for (int i = 0; i < len; i++) {
		if (id == ids[i]) {
			return true;
		}
	}
	return false;
}

bool block_isOpaqueBlock(BlockID id) {
	BlockID blocks[] = {
		BLOCK_Unknown, // yes, and we even have a texture for it
		BLOCK_GrassBlock, BLOCK_Dirt, BLOCK_Stone, BLOCK_CobbleStone,
	};
	return inArray(id, blocks, ARRLEN(blocks));
}

bool block_isPlant(BlockID id) {
	BlockID plants[] = {
		BLOCK_Poppy, BLOCK_Dandelion,
	};
	return inArray(id, plants, ARRLEN(plants));
}
