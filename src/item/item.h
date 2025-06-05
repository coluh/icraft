#ifndef _ICRAFT_item_item_h
#define _ICRAFT_item_item_h

#include <stdbool.h>

typedef enum ItemID {
	ITEM_Unknown, // TODO: ITEM_Empty
	ITEM_Dirt,
	ITEM_CobbleStone,
	ITEM_GrassBlock,
	ITEM_Stone,
	ITEM_Poppy,
	ITEM_Dandelion,
	ITEM_Bucket,
	ITEM_WaterBucket,
	ITEM_OakLog,
	ITEM_OakPlank,
	ITEM_OakLeave,
	ITEM_CraftingTable,
} ItemID;

typedef struct Item {
	ItemID id; // base id

	// these have default values according to the id
	const char *name;
} Item;

typedef struct Slot {
	Item item;
	int count;
} Slot;

const char *item_name(Item *item);

// item is cube, can be put, show 3d texture in inventory
// used in: inventory item render, drops entity render
bool item_is3d(ItemID id);

// return an opengl texture
// used in: inventory, container ui
unsigned int item_3dIconTexture(ItemID id);

// return index in the g.res->textures.blocks texture atlas
// keep the same with block_types.textures
// used in: inventory, container ui
int item_textureIndex(ItemID id);

// just put the corresponding block
// used in: blockput
bool item_putable(ItemID id);

#endif
