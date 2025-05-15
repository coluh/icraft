#ifndef _ICRAFT_world_block_extra_h
#define _ICRAFT_world_block_extra_h

typedef enum BlockExtraType {
	BlockExtra_DESTROY,
	BlockExtra_CHEST,
} BlockExtraType;

typedef struct BlockExtra {
	int x, y, z;
	BlockExtraType type;
	union {
		struct {
			float progress;
		} destroying;
	};
} BlockExtra;

typedef struct World World;
BlockExtra *newBlockExtra(World *world, int x, int y, int z, BlockExtraType type);

// usage example:
void block_destroy(World *world, int x, int y, int z);
void block_destroyCallback(World *world, int x, int y, int z);

#endif
