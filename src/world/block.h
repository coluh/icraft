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

// used in: chunk water vertex generation face culling
#define BLOCK_TRANSPARENT (1 << 0)
#define BLOCK_ISTRANSPARENT(block) (block_get(block)->properties & BLOCK_TRANSPARENT)
// used in: chunk vertex generation
#define BLOCK_PLANT (1 << 1)
#define BLOCK_ISPLANT(block) (block_get(block)->properties & BLOCK_PLANT)
// used in: water placement and expand, plant destroy update(just now)
#define BLOCK_WATERABLE (1 << 2)
#define BLOCK_ISWATERABLE(block) (block_get(block)->properties & BLOCK_WATERABLE)

typedef struct BlockType {

	const char *name;

	// texture of 6 faces, following -z, z, -y, y, -x, x
	int textures[6];

	const char *break_sound;
	float break_time;
	ItemID break_item;

	unsigned int properties;
} BlockType;

void block_init();

// get detailed data of this type of block
const BlockType *block_get(BlockID id);

// used in: ? command
BlockID block_idOf(const char *name);

// itemID -> BlockID
// used in: put block, drops render
BlockID block_ofItem(ItemID item);

#endif
