#ifndef _ICRAFT_render_render_h
#define _ICRAFT_render_render_h

#include "../camera/camera.h"
#include "../world/world.h"

void render_init();
int render_getFPS();

void render(Camera *camera, const World *world, float alpha);

#endif
