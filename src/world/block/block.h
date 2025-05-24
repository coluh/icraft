#ifndef _ICRAFT_world_block_block_h
#define _ICRAFT_world_block_block_h

#include <stdint.h>
#include "../../item/item.h"

#define BLOCK_TEXTURE_SIZE 16
// How many textures in one row. Also column count
#define BLOCK_TEXTURE_ROW_COUNT 16

typedef enum BlockID {
	BLOCK_Air,
	BLOCK_GrassBlock,
	BLOCK_Dirt,
	BLOCK_Stone,
	BLOCK_CobbleStone,
	BLOCK_Poppy,
	BLOCK_Dandelion,
	BLOCK_Unknown,
} BlockID;

typedef enum FaceIndex {
	// -z, z, -y, y, -x, x
	Face_BACK, Face_FRONT, Face_BOTTOM, Face_TOP, Face_LEFT, Face_RIGHT,
} FaceIndex;

typedef struct BlockType {

	const char *name;

	// texture of 6 faces, following -z, z, -y, y, -x, x
	int textures[6];

	const char *break_sound;
	float break_time;
	ItemID break_item;
} BlockType;

void block_init();
const BlockType *block_get(BlockID id);

BlockID block_idOf(const char *name);
BlockID block_ofItem(ItemID item);

bool block_isOpaqueBlock(BlockID id);
bool block_isPlant(BlockID id);

#endif
