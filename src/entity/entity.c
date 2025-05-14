#include "entity.h"
#include <string.h>
#include "../game.h"
#include "../util/mem.h"
#include "../../third_party/cglm/include/cglm/quat.h"
#include "item.h"
#include "player.h"

#define ENTITY_LIST_INIT_CAP 32

extern Game g;

void entity_init() {
	EntityList *l = zalloc(1, sizeof(EntityList));
	l->list = zalloc(ENTITY_LIST_INIT_CAP, sizeof(Entity));
	l->capacity = ENTITY_LIST_INIT_CAP;
	g.entities = l;
}

static void entity_expand(EntityList *l) {
	int newcap = l->capacity;
	if (l->capacity < 256) {
		newcap *= 2;
	} else {
		newcap += (newcap + 3 * 256) / 4;
	}
	rezalloc(l->list, newcap, sizeof(Entity));
	// zalloc to ensure new entity->active == false
	l->capacity = newcap;
}

static void (*update_of(EntityType type))(Entity *self, World *world) {
	switch (type) {
	case ENTITY_player:
		return player_update;
	case ENTITY_item:
		return item_update;
	default:
		return NULL;
	}
}

static void (*render_of(EntityType type))(Entity *self, float alpha) {
	switch (type) {
	case ENTITY_player:
		return NULL;
	case ENTITY_item:
		return item_render;
	default:
		return NULL;
	}
}

Entity *entity_create(EntityType type, V3 position) {
	EntityList *l = g.entities;
	for (int i = 0; i < l->capacity; i++) {
		if (!l->list[i].active) {
			Entity *ep = &l->list[i];
			memset(ep, 0, sizeof(Entity));
			ep->active = true;

			ep->type = type;
			ep->position = position;
			ep->velocity = (V3){0, 0, 0};
			glm_quat_identity(ep->rotation);
			ep->update = update_of(type);
			ep->render = render_of(type);

			switch (ep->type) {
			case ENTITY_player:
				player_init(ep);
				break;
			case ENTITY_item:
				item_init(ep);
				break;
			default:
				break;
			}

			return ep;
		}
	}
	entity_expand(l);
	return entity_create(type, position);

	// generation array...
}

void entity_delete(Entity *entity) {
	EntityList *l = g.entities;
	for (int i = 0; i < l->capacity; i++) {
		if (&l->list[i] == entity) {
			l->list[i].active = false;
		}
	}
}

void entity_update(World *w) {
	for (int i = 0; i < g.entities->capacity; i++) {
		Entity *ep = &g.entities->list[i];
		if (ep->active && ep->update) {
			ep->update(ep, w);
		}
	}
}

void entity_render(float alpha) {
	for (int i = 0; i < g.entities->capacity; i++) {
		Entity *ep = &g.entities->list[i];
		if (ep->active && ep->render) {
			ep->render(ep, alpha);
		}
	}
}
