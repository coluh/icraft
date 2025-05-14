#ifndef _ICRAFT_entity_item_h
#define _ICRAFT_entity_item_h

#include "../../third_party/cglm/include/cglm/types.h"

typedef struct ItemData {
	float scale;
	float float_timer;
	float rotate_timer;
	vec3 render_position;
	versor render_rotation;
	vec3 previous_position;
	versor previous_rotation;
} ItemData;

typedef struct Entity Entity;
typedef struct World World;
typedef struct Camera Camera;
void item_init(Entity *entity);
void item_update(Entity *entity, World *w);
void item_render(Entity *entity, float alpha);

#endif
