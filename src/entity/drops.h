#ifndef _ICRAFT_entity_drops_h
#define _ICRAFT_entity_drops_h

#include "../../third_party/cglm/include/cglm/types.h"
#include "../item/item.h"

typedef struct DropsData {
	float scale;
	float float_timer;
	float rotate_timer;
	vec3 render_position;
	versor render_rotation;
	vec3 previous_position;
	versor previous_rotation;

	ItemID item;
} DropsData;

typedef struct Entity Entity;
typedef struct World World;
typedef struct Camera Camera;
void drops_init(Entity *entity);
void drops_update(Entity *entity, World *w);
void drops_render(Entity *entity, float alpha);

#endif
