#include "extralist.h"
#include "../util/mem.h"

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
			free(n->extra); // seems the first free in proj
			free(n);
		}
	}
	logw("No such node: %p", extra);
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
