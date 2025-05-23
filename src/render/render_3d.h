#ifndef _ICRAFT_render_render3d_h
#define _ICRAFT_render_render3d_h

#include "../world/world.h"

void threed_renderChunks(const World *world);
void threed_renderExtras(const World *world);
void threed_renderFacing();

#endif
