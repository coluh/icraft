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

// below are two use examples

void block_destroy(World *world, int x, int y, int z) {

	// block -> entity
	BlockID origin = world_modifyBlock(world, x, y, z, BLOCK_Air);
	Entity *drops = entity_get(g.entities, entity_create(Entity_DROPS, (V3){x+0.5f, y+0.5f, z+0.5f}, g.entities));
	drops->drops.item = block_get(origin)->break_item;
	drops->velocity.x = rand_float(-2.0f, 2.0f);
	drops->velocity.y = rand_float(2.0f, 5.0f);
	drops->velocity.z = rand_float(-2.0f, 2.0f);
}

void block_destroyCallback(World *world, int x, int y, int z) {

	Chunk *c = world_chunkAti(world, x, y, z);
	BlockExtra *e = extralist_find(c->extras, x, y, z, BlockExtra_DESTROY);
	if (e == NULL) {
		e = newBlockExtra(world, x, y, z, BlockExtra_DESTROY);
	}

	e->destroying.time += g.update_delta;
	e->destroying.focus = true;

	if (e->destroying.time >= block_get(world_block(world, x, y, z))->break_time) {
		extralist_remove(&c->extras, e);
		block_destroy(world, x, y, z);
	}
}
