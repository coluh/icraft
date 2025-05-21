#ifndef _ICRAFT_entity_entity_h
#define _ICRAFT_entity_entity_h

#include "../util/types.h"
#include <stdbool.h>
#include "../../third_party/cglm/include/cglm/types.h"

#include "player.h"
#include "drops.h"

#define DROPS_ATTRACTION_DISTANCE_SQUARE 2.0f

typedef enum EntityType {
	Entity_PLAYER,
	Entity_DROPS,
} EntityType;

typedef struct World World;
typedef struct Camera Camera;

typedef struct Entity {

	// data need for reuse
	bool active; // mean used
	unsigned int generation;

	// this is designed for entity that is no more updated but still rendered in a short time
	// every entity should consider this in their update functions
	bool only_render;
	// bool no_update;

	EntityType type;

	V3 position;
	V3 velocity;
	versor rotation;
	bool on_ground; // may this should be in extra data field
	// bool no_physics;

	void (*update)(struct Entity *self, World *world);
	void (*render)(struct Entity *self, float alpha);

	union {
		PlayerData player;
		DropsData drops;
	};
} Entity;

typedef struct EntityList {
	Entity *data;
	int capacity;
} EntityList;

EntityList *newEntityList();

PoolHandle entity_create(EntityType type, V3 position, EntityList *list);
Entity *entity_get(EntityList *list, PoolHandle ref);
void entity_delete(EntityList *list, Entity *entity);

void entity_update(EntityList *list, World *world);
void entity_render(const EntityList *list, float alpha);

#endif
