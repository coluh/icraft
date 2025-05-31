#include "blockstate.h"
#include "../world.h"
#include "blockstatelist.h"
#include "../../util/log.h"

BlockState *blockstate_add(BlockState *state, World *w, int x, int y, int z) {
	if (state->type == BlockState_WATER)
		logd("Add block state at %d %d %d of water level %d", x, y, z, state->water.level);
	Chunk *chunk = world_chunkAti(w, x, y, z);
	x -= chunk->x;
	y -= chunk->y;
	z -= chunk->z;
	blockstatelist_add(&chunk->block_states, x, y, z, state);
	return blockstatelist_get(&chunk->block_states, x, y, z);
}

BlockState *blockstate_get(World *w, int x, int y, int z) {
	Chunk *chunk = world_chunkAti(w, x, y, z);
	x -= chunk->x;
	y -= chunk->y;
	z -= chunk->z;
	return blockstatelist_get(&chunk->block_states, x, y, z);
}

BlockState *blockstate_getByType(World *w, int x, int y, int z, BlockStateType type) {
	Chunk *chunk = world_chunkAti(w, x, y, z);
	x -= chunk->x;
	y -= chunk->y;
	z -= chunk->z;
	return blockstatelist_getByType(&chunk->block_states, x, y, z, type);
}

void blockstate_removeAll(World *w, int x, int y, int z) {
	Chunk *chunk = world_chunkAti(w, x, y, z);
	x -= chunk->x;
	y -= chunk->y;
	z -= chunk->z;
	blockstatelist_removeAll(&chunk->block_states, x, y, z);
}

void blockstate_removeByType(World *w, int x, int y, int z, BlockStateType type) {
	Chunk *chunk = world_chunkAti(w, x, y, z);
	x -= chunk->x;
	y -= chunk->y;
	z -= chunk->z;
	blockstatelist_removeByType(&chunk->block_states, x, y, z, type);
}

void blockstate_removeByValue(World *w, int x, int y, int z, const BlockState *v) {
	Chunk *chunk = world_chunkAti(w, x, y, z);
	x -= chunk->x;
	y -= chunk->y;
	z -= chunk->z;
	blockstatelist_removeByValue(&chunk->block_states, x, y, z, v);
}
