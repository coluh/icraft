#ifndef _ICRAFT_item_item_h
#define _ICRAFT_item_item_h

#include <stdbool.h>

typedef enum ItemID {
	ITEM_Unknown,
	ITEM_Dirt,
	ITEM_CobbleStone,
	ITEM_GrassBlock,
	ITEM_Stone,
	ITEM_Poppy,
	ITEM_Dandelion,
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

// item is cube, can be put, show 3d texture in inventory
bool item_isCube(ItemID id);

// return an opengl texture
// used in: inventory, container ui
unsigned int item_cubeIconTexture(ItemID id);

// return index in the g.res->textures.blocks texture
// keep the same with block_types.textures
// used in: inventory, container ui
int item_textureIndex(ItemID id);

// used in: block put
bool item_putable(ItemID id);

#endif
