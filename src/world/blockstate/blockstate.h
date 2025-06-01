#ifndef _ICRAFT_world_blockstate_blockstate_h
#define _ICRAFT_world_blockstate_blockstate_h

#include <stdbool.h>

#define WATER_SOURCE 8

typedef enum BlockStateType {
	BlockState_DESTROY,
	BlockState_WATER,
} BlockStateType;

typedef enum FluidDirection {
	FluidDirection_XP,
	FluidDirection_XPZP,
	FluidDirection_ZP,
	FluidDirection_ZPXN,
	FluidDirection_XN,
	FluidDirection_XNZN,
	FluidDirection_ZN,
	FluidDirection_ZNXP,
} FluidDirection;

typedef struct BlockState {
	BlockStateType type;
	union {
		struct BlockStateDestroy {
			float time;
			float total;
			bool focus;
		} destroy;
		struct BlockStateWater {
			int level; // in 1 ~ 7, 8 means source
			FluidDirection dir;
		} water;
	};
} BlockState;

typedef struct World World;
// add state to world coord x, y, z
// state will be copy to chunk->BlockStateList
// return final valid state
BlockState *blockstate_add(BlockState *state, World *w, int x, int y, int z);
// get state at world coord x, y, z
BlockState *blockstate_get(World *w, int x, int y, int z);
// get state of type type at world coord x, y, z
BlockState *blockstate_getByType(const World *w, int x, int y, int z, BlockStateType type);

// remove all states in block x, y, z
void blockstate_removeAll(World *w, int x, int y, int z);
// remove all states of type type in block x, y, z
void blockstate_removeByType(World *w, int x, int y, int z, BlockStateType type);
// remove the specified state in block x, y, z
void blockstate_removeByValue(World *w, int x, int y, int z, const BlockState *v);

#endif
