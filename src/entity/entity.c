#include "entity.h"
#include <string.h>
#include "../util/mem.h"
#include "../../third_party/cglm/include/cglm/quat.h"
#include "bodies.h"
#include "drops.h"
#include "player.h"
#include "../timer/timer.h"

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
	logi("entity already died");
	return NULL;
}

void entity_delete(EntityList *l, Entity *entity) {
	for (int i = 0; i < l->capacity; i++) {
		if (&l->data[i] == entity) {
			l->data[i].active = false;
		}
	}
}

struct EntityMove {
	Entity *a;
	V3 b_pos;
};
static bool moveEntityCallback(void *user_data) {
	Entity *a = ((struct EntityMove*)user_data)->a;
	V3 b_pos = ((struct EntityMove*)user_data)->b_pos;
	vec3 dir;
	glm_vec3_sub((float*)&b_pos, (float*)&a->position, dir);
	if (glm_vec3_norm2(dir) < 0.25f) {
		a->active = false;
		return true;
	}
	const float decay = 0.7f;
	glm_vec3_scale(dir, 1.0f - decay, dir);
	glm_vec3_add((float*)&a->position, dir, (float*)&a->position);
	return false;
}

// update every entities;
// perform attraction of drops towards player;
void entity_update(EntityList *l, World *w) {

	for (int i = 0; i < l->capacity; i++) {
		Entity *ep = &l->data[i];
		if (ep->active && ep->update) {
			ep->update(ep, w);
		}
	}

	// attraction
	for (int i = 0; i < l->capacity; i++) {
		for (int j = 0; j < l->capacity; j++) {
			Entity *a = &l->data[i];
			Entity *b = &l->data[j];
			if (!a->active || !b->active || a->only_render || b->only_render) {
				continue;
			}
			if (a->type == Entity_DROPS && b->type == Entity_PLAYER && glm_vec3_distance2((float*)&a->position, (float*)&b->position) < DROPS_ATTRACTION_DISTANCE_SQUARE) {
				if (a->drops.pickup_timer > 0) { // not now
					continue;
				}
				// player get the drops immediately, while there is an animation of drops flying to player
				a->only_render = true;
				player_pickup(b, a->drops.item);
				timer_schedule_repeat(g.update_delta, moveEntityCallback, &(struct EntityMove){
						a, {b->position.x, b->position.y+PLAYER_EYE_OFFSET_Y, b->position.z}
				}, sizeof(struct EntityMove));
			}
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
