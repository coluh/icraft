#ifndef _ICRAFT_effects_particle_h
#define _ICRAFT_effects_particle_h

/* particles do not influence any game logic */

#include <stdbool.h>
#include "../util/types.h"

typedef struct Particle {
	bool alive;
	V3 pos;
	V3 pre_pos;
	V3 v;
	float lifetime;
	float age;
	float size;
	int texture_index;
} Particle;

// return pointer to the Particle struct, but
// NOTE: this pointer can only be temporatily used to like set lifetime or size
// you should not keep the pointer because address can change when inner expand
// happens
Particle *particle_spawn(float x, float y, float z, int texture_index);

void particle_update();
typedef struct Camera Camera;
void particle_render(Camera *camera, float alpha);

#endif
