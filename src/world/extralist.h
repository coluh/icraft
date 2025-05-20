#ifndef _ICRAFT_world_extralist_h
#define _ICRAFT_world_extralist_h

#include "block/extra.h"
#include <stdbool.h>

typedef struct BlockExtraNode {
	BlockExtra *extra;
	bool delete_me;
	struct BlockExtraNode *next;
} BlockExtraNode, *BlockExtraList;

void extralist_add(BlockExtraList *list, BlockExtra *extra);
void extralist_remove(BlockExtraList *list, BlockExtra *extra);
void extralist_autoRemove(BlockExtraList *list);
BlockExtra *extralist_find(BlockExtraList list, int x, int y, int z, BlockExtraType type);

#endif
