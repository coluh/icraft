#ifndef _ICRAFT_entity_entity_h
#define _ICRAFT_entity_entity_h

#include "../util/types.h"
#include <stdbool.h>
#include "../../third_party/cglm/include/cglm/types.h"

#include "player.h"
#include "drops.h"

typedef enum EntityType {
	Entity_PLAYER,
	Entity_DROPS,
} EntityType;

typedef struct World World;
typedef struct Camera Camera;

typedef struct Entity {

	bool active; // mean used
	EntityType type;

	V3 position;
	V3 velocity;
	versor rotation;
	bool on_ground;

	void (*update)(struct Entity *self, World *world);
	void (*render)(struct Entity *self, float alpha);

	union {
		PlayerData player;
		DropsData drops;
	};
} Entity;

typedef struct EntityList {
	Entity *list;
	int capacity;
} EntityList;

void entity_init();
Entity *entity_create(EntityType type, V3 position);
void entity_delete(Entity *entity);

void entity_update(World *world);
void entity_render(float alpha);

#endif
