#ifndef _ICRAFT_render_render_h
#define _ICRAFT_render_render_h

#include "../player/camera.h"
#include "../world/world.h"

void render_init();
int render_getFPS();

void render(Camera *camera, World *world);

#endif
