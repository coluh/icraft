#include "extralist.h"
#include "../util/mem.h"
#include "../entity/entity.h"

void extralist_add(BlockExtraList *list, BlockExtra *extra) {
	BlockExtraNode *node = zalloc(1, sizeof(BlockExtraNode));
	node->extra = extra;
	// for (BlockExtraNode **p = list; p != NULL; p = &(*p)->next)

	BlockExtraNode **p = list;
	while (*p != NULL)
		p = &(*p)->next;

	*p = node;
}

void extralist_remove(BlockExtraList *list, BlockExtra *extra) {
	for (BlockExtraNode **p = list; *p != NULL; p = &(*p)->next) {
		if ((*p)->extra == extra) {
			BlockExtraNode *n = *p;
			*p = n->next;
			free(n->extra);
			free(n);
			return;
		}
	}
	logw("No such node: %p", extra);
}

void extralist_update(BlockExtraList *list) {
	// blockextra states
	for (BlockExtraNode *bp = *list; bp != NULL; bp = bp->next) {
		BlockExtra *be = bp->extra;
		if (be->type == BlockExtra_DESTROY) {
			const Entity *player = entity_get(g.entities, g.player_ref);
			if (!be->destroying.focus ||
					(player->player.facing_block.x != be->x) ||
					(player->player.facing_block.y != be->y) ||
					(player->player.facing_block.z != be->z)) {
				bp->delete_me = true;
			}
			be->destroying.focus = false;
		}
	}
	extralist_autoRemove(list);
}

void extralist_autoRemove(BlockExtraList *list) {
	for (BlockExtraNode **p = list; *p != NULL;) {
		if ((*p)->delete_me) {
			BlockExtraNode *n = *p;
			*p = n->next;
			free(n->extra);
			free(n);
			continue;
		}
		p = &(*p)->next;
	}
}

BlockExtra *extralist_find(BlockExtraList list, int x, int y, int z, BlockExtraType type) {
	for (BlockExtraNode *p = list; p != NULL; p = p->next) {
		BlockExtra *e = p->extra;
		if (e->x == x && e->y == y && e->z == z && e->type == type) {
			return e;
		}
	}
	return NULL;
}
