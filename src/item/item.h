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

unsigned int item_blockTexture(ItemID id);
bool item_isBlock(ItemID id);

#endif
