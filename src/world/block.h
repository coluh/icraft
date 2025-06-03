#ifndef _ICRAFT_world_block_block_h
#define _ICRAFT_world_block_block_h

#include <stdint.h>
#include "../item/item.h"

/* block is not cube */
/* block is the basic unit of the chunk */

typedef enum BlockID {
	BLOCK_Air,
	BLOCK_GrassBlock,
	BLOCK_Dirt,
	BLOCK_Stone,
	BLOCK_CobbleStone,
	BLOCK_Poppy,
	BLOCK_Dandelion,
	BLOCK_OakLog,
	BLOCK_OakPlank,
	BLOCK_OakLeave,
	BLOCK_CraftingTable,
	BLOCK_Unknown,
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

// get detailed data of this type of block
const BlockType *block_get(BlockID id);

// used in: ? command
BlockID block_idOf(const char *name);

// itemID -> BlockID
// used in: put block, drops render
BlockID block_ofItem(ItemID item);

// used in: chunk vertex generation, water render
bool block_isOpaqueBlock(BlockID id);
// used in: chunk vertex generation
bool block_isPlant(BlockID id);
// complete solid cannot contain water
// used in: water expand
bool block_isCompleteSolid(BlockID id);

#endif
