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
	BLOCK_Unknown
} BlockID;

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

#endif
