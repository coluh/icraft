#ifndef _ICRAFT_world_blockstate_blockstatelist_h
#define _ICRAFT_world_blockstate_blockstatelist_h

// NOTE: each position can have many block states of different types!
// may have more than 1 state of the same time in the same position

#include "blockstate.h"

typedef struct BlockStateNode {
	int x, y, z; // NOTE: local coord in chunk space
	bool remove_me;
	BlockState state;
} BlockStateNode;

typedef struct BlockStateList {
	BlockStateNode *data;
	int length;
	int capacity;
} BlockStateList;

BlockState *blockstatelist_get(BlockStateList *list, int x, int y, int z);
BlockState *blockstatelist_getByType(BlockStateList *list, int x, int y, int z, BlockStateType type);
void blockstatelist_add(BlockStateList *list, int x, int y, int z, BlockState *state);
typedef struct Chunk Chunk;
void blockstatelist_update(BlockStateList *list, Chunk *chunk, World *world);

// remove all states in block x, y, z
void blockstatelist_removeAll(BlockStateList *list, int x, int y, int z);
// remove all states of type type in block x, y, z
void blockstatelist_removeByType(BlockStateList *list, int x, int y, int z, BlockStateType type);
// remove the specified state in block x, y, z
void blockstatelist_removeByValue(BlockStateList *list, int x, int y, int z, const BlockState *v);

#endif
