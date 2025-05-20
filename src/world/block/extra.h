#ifndef _ICRAFT_world_block_extra_h
#define _ICRAFT_world_block_extra_h

#include <stdbool.h>

typedef enum BlockExtraType {
	BlockExtra_DESTROY,
	BlockExtra_CHEST,
} BlockExtraType;

typedef struct BlockExtra {
	int x, y, z;
	BlockExtraType type;
	union {
		struct {
			float time;
			bool focus;
		} destroying;
	};
} BlockExtra;

typedef struct World World;
BlockExtra *newBlockExtra(World *world, int x, int y, int z, BlockExtraType type);

void block_destroy(World *world, int x, int y, int z);
void block_destroyCallback(World *world, int x, int y, int z);

#endif
