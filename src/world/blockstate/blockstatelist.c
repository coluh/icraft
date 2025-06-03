#include "blockstatelist.h"
#include <stddef.h>
#include <string.h>
#include "../../util/mem.h"
#include "blockstate.h"
#include "../chunk.h"
#include "../world.h"
#include "../../util/dynarray.h"
#include "../../util/props.h"
// #include "../../util/log.h"

static void expand(BlockStateList *l) {
	int newcap = l->capacity;
	newcap = (l->capacity < 256) ? (newcap*2) : (newcap*5/4 + 192);
	l->data = rezalloc(l->data, newcap, sizeof(BlockStateNode));
	l->capacity = newcap;
}

BlockState *blockstatelist_get(BlockStateList *list, int x, int y, int z) {
	for (int i = 0; i < list->length; i++) {
		BlockStateNode *n = &list->data[i];
		if (n->x == x && n->y == y && n->z == z) {
			return &n->state;
		}
	}
	return NULL;
}

BlockState *blockstatelist_getByType(BlockStateList *list, int x, int y, int z, BlockStateType type) {
	for (int i = 0; i < list->length; i++) {
		BlockStateNode *n = &list->data[i];
		if (n->x == x && n->y == y && n->z == z && n->state.type == type) {
			return &n->state;
		}
	}
	return NULL;
}

BlockState *blockstatelist_add(BlockStateList *l, int x, int y, int z, BlockState *state) {
	if (l->data == NULL) {
		// init
		l->capacity = 8;
		l->data = zalloc(l->capacity, sizeof(BlockStateNode));
	}
	BlockStateNode *p = &l->data[l->length];
	p->x = x;
	p->y = y;
	p->z = z;
	p->remove_me = false;
	p->state = *state;

	l->length++;

	if (l->length == l->capacity) {
		expand(l);
	}

	return &p->state;
}

void blockstatelist_removeAll(BlockStateList *l, int x, int y, int z) {
	for (int i = 0; i < l->length; i++) {
		BlockStateNode *n = &l->data[i];
		if (n->x == x && n->y == y && n->z == z) {
			n->remove_me = true;
		}
	}
}

void blockstatelist_removeByType(BlockStateList *l, int x, int y, int z, BlockStateType type) {
	for (int i = 0; i < l->length; i++) {
		BlockStateNode *n = &l->data[i];
		if (n->x == x && n->y == y && n->z == z && n->state.type == type) {
			n->remove_me = true;
		}
	}
}

void blockstatelist_removeByValue(BlockStateList *l, int x, int y, int z, const BlockState *v) {
	for (int i = 0; i < l->length; i++) {
		BlockStateNode *n = &l->data[i];
		if (n->x == x && n->y == y && n->z == z && &n->state == v) {
			n->remove_me = true;
		}
	}
}

// struct PreparedStateData {
// 	BlockState s;
// 	World *w;
// 	int x, y, z;
// };
//
// static void add_state(void *user_data) {
// 	struct PreparedStateData *s = user_data;
// 	blockstate_add(&s->s, s->w, s->x, s->y, s->z);
// }

static void blockstatelist_autoRemove(BlockStateList *l) {
	// auto remove
	for (int i = l->length-1; i >= 0; i--) {
		BlockStateNode *n = &l->data[i];
		if (n->remove_me) {
			if (i != l->length-1) {
				memmove(n, n + 1, (l->length - 1 - i)*sizeof(BlockStateNode));
				// so do not keep pointer to state by hand
			}
			l->length--;
		}
	}
}

void blockstatelist_update(BlockStateList *l, Chunk *c, World *w) {

	// update
	for (int i = 0; i < l->length; i++) {
		BlockStateNode *n = &l->data[i];
		BlockState *state = &n->state;
		switch (state->type) {
		case BlockState_DESTROY:
			if (!state->destroy.focus) {
				n->remove_me = true; // you lost focus on me. I'll remove myself
			}
			state->destroy.focus = false; // if you don't want me go, focus on me every update
			break;
		case BlockState_WATER:
			if (g.ticks % 5 == 0) {
				int x = c->x + n->x;
				int y = c->y + n->y;
				int z = c->z + n->z;

				// decline
				bool has_higher = false;
				const int dirs[][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
				for (int d = 0; d < 4; d++) {
					int nx = x + dirs[d][0];
					int nz = z + dirs[d][1];
					const BlockState *ns = blockstate_getByType(w, nx, y, nz, BlockState_WATER);
					if (ns && ns->water.level > state->water.level) {
						has_higher = true;
					}
				}
				if (!has_higher) {
					// try up block
					const BlockState *ns = blockstate_getByType(w, x, y + 1, z, BlockState_WATER);
					if (ns && ns->water.level > 0) {
						has_higher = true;
					}
				}
				if (!has_higher && state->water.level != WATER_SOURCE) {
					state->water.level--;
				}
				if (state->water.level <= 0) {
					n->remove_me = true;
					break;
				}
			}
			break;
		default:
			break;
		}
	}

	blockstatelist_autoRemove(l);

	DynArray *da = dynarray_create(4*sizeof(int)); // pos, level
	for (int i = 0; i < l->length; i++) {
		BlockStateNode *n = &l->data[i];
		if (n->state.type != BlockState_WATER) {
			continue;
		}
		if (g.ticks % 5 != 0) {
			continue;
		}
		int x = c->x + n->x;
		int y = c->y + n->y;
		int z = c->z + n->z;
		struct BlockStateWater self = n->state.water;
		const int dirs[][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

		// collect new water positions
		BlockState *bs = blockstate_getByType(w, x, y - 1, z, BlockState_WATER);
		if (bs == NULL && !block_isCompleteSolid(world_block(w, x, y - 1, z))) {
			dynarray_push(da, (int[]){x, y - 1, z, 7});
		} else if (n->state.water.level > 1 && block_isCompleteSolid(world_block(w, x, y - 1, z))){
			for (int d = 0; d < 4; d++) {
				int nx = x + dirs[d][0];
				int nz = z + dirs[d][1];
				BlockState *ns = blockstate_getByType(w, nx, y, nz, BlockState_WATER);
				// TODO: not opaque, but solid
				if (ns == NULL && !block_isOpaqueBlock(world_block(w, nx, y, nz))) {
					dynarray_push(da, (int[]){nx, y, nz, self.level - 1});
				}
			}
		}

		// update water levels
		BlockState *ups = blockstate_getByType(w, x, y + 1, z, BlockState_WATER);
		if (ups) {
			self.level = 7;
		} else {
			for (int d = 0; d < 4; d++) {
				int nx = x + dirs[d][0];
				int nz = z + dirs[d][1];
				BlockState *ns = blockstate_getByType(w, nx, y, nz, BlockState_WATER);
				if (ns && self.level < ns->water.level - 1) {
					self.level = ns->water.level - 1;
				}
			}
		}
	}

	// create new waters
	// TODO: generate new water source near two water source
	for (int i = 0; i < da->size; i++) {
		int *xyzl = da->data + i*da->elem_size;
		int x = xyzl[0];
		int y = xyzl[1];
		int z = xyzl[2];
		int l = xyzl[3];
		// if repeated, choose higher one
		BlockState *s = blockstate_getByType(w, x, y, z, BlockState_WATER);
		if (s) {
			s->water.level = MAX(s->water.level, l);
		} else {
			BlockState ns = { .type = BlockState_WATER, .water = { .level = l }};
			blockstate_add(&ns, w, x, y, z);
		}
	}

	dynarray_free(da);
}
