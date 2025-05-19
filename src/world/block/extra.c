#include "extra.h"
#include "../../util/mem.h"
#include "../../util/props.h"
#include "../extralist.h"
#include "../world.h"
#include "block.h"
#include "../../entity/entity.h"

BlockExtra *newBlockExtra(World *world, int x, int y, int z, BlockExtraType type) {
	BlockExtra *be = zalloc(1, sizeof(BlockExtra));
	be->x = x;
	be->y = y;
	be->z = z;
	be->type = type;
	extralist_add(&world_chunkAti(world, x, y, z)->extras, be);
	return be;
}

void block_destroy(World *world, int x, int y, int z) {
	world_modifyBlock(world, x, y, z, BLOCK_Air);

	Entity *item = entity_create(ENTITY_item, (V3){x+0.5f, y+0.5f, z+0.5f});
	item->velocity.x = rand_float(-2.0f, 2.0f);
	item->velocity.y = rand_float(2.0f, 5.0f);
	item->velocity.z = rand_float(-2.0f, 2.0f);
}

void block_destroyCallback(World *world, int x, int y, int z) {
	Chunk *c = world_chunkAti(world, x, y, z);
	BlockExtra *e = extralist_find(c->extras, x, y, z, BlockExtra_DESTROY);
	if (e == NULL) {
		e = newBlockExtra(world, x, y, z, BlockExtra_DESTROY);
	}

	e->destroying.progress += 0.025;

	if (e->destroying.progress >= 1.0f) {
		extralist_remove(&c->extras, e);
		block_destroy(world, x, y, z);
	}
}
