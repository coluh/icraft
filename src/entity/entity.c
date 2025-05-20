#include "entity.h"
#include <string.h>
#include "../util/mem.h"
#include "../../third_party/cglm/include/cglm/quat.h"
#include "drops.h"
#include "player.h"

#define ENTITY_LIST_INIT_CAP 32

EntityList *newEntityList() {
	EntityList *l = zalloc(1, sizeof(EntityList));
	l->data = zalloc(ENTITY_LIST_INIT_CAP, sizeof(Entity));
	l->capacity = ENTITY_LIST_INIT_CAP;
	return l;
}

static void entity_expand(EntityList *l) {
	int newcap = l->capacity;
	if (l->capacity < 256) {
		newcap *= 2;
	} else {
		newcap += (newcap + 3 * 256) / 4;
	}
	l->data = rezalloc(l->data, newcap, sizeof(Entity));
	// zalloc to ensure new entity->active == false
	l->capacity = newcap;
}

static void (*update_of(EntityType type))(Entity *self, World *world) {
	switch (type) {
	case Entity_PLAYER:
		return player_update;
	case Entity_DROPS:
		return drops_update;
	default:
		return NULL;
	}
}

static void (*render_of(EntityType type))(Entity *self, float alpha) {
	switch (type) {
	case Entity_PLAYER:
		return NULL;
	case Entity_DROPS:
		return drops_render;
	default:
		return NULL;
	}
}

// return the index in entity list array
PoolHandle entity_create(EntityType type, V3 position, EntityList *l) {
	for (int i = 0; i < l->capacity; i++) {
		if (!l->data[i].active) {
			Entity *ep = &l->data[i];
			memset(ep, 0, sizeof(Entity));
			ep->active = true;
			ep->generation++;

			ep->type = type;
			ep->position = position;
			ep->velocity = (V3){0, 0, 0};
			glm_quat_identity(ep->rotation);
			ep->update = update_of(type);
			ep->render = render_of(type);

			switch (ep->type) {
			case Entity_PLAYER:
				player_init(ep);
				break;
			case Entity_DROPS:
				drops_init(ep);
				break;
			default:
				break;
			}

			return (PoolHandle){i, ep->generation};
		}
	}
	entity_expand(l);
	return entity_create(type, position, l);
}

Entity *entity_get(EntityList *l, PoolHandle ref) {
	Entity *e = &l->data[ref.index];
	if (e != NULL && e->generation == ref.generation) {
		return e;
	}

	// the entity you reference to is already dead
	return NULL;
}

void entity_delete(EntityList *l, Entity *entity) {
	for (int i = 0; i < l->capacity; i++) {
		if (&l->data[i] == entity) {
			l->data[i].active = false;
		}
	}
}

// update every entities;
// perform attraction of drops towards player
void entity_update(EntityList *l, World *w) {

	for (int i = 0; i < l->capacity; i++) {
		Entity *ep = &l->data[i];
		if (ep->active && ep->update) {
			ep->update(ep, w);
		}
	}

	// attraction
	for (int i = 0; i < g.entities->capacity; i++) {
		for (int j = 0; j < g.entities->capacity; j++) {
			;
		}
	}
}

void entity_render(const EntityList *l, float alpha) {
	for (int i = 0; i < g.entities->capacity; i++) {
		Entity *ep = &l->data[i];
		if (ep->active && ep->render) {
			ep->render(ep, alpha);
		}
	}
}
