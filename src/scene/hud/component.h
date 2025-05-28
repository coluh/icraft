#ifndef _ICRAFT_scene_hud_component_h
#define _ICRAFT_scene_hud_component_h

#include "../../render/render_2d.h"

// x, y is outline coord
static inline void bar(int x, int y, int w, int h, int border) {
	twod_drawQuad(x, y, w, border);
	twod_drawQuad(x, y + h - border, w, border);
	twod_drawQuad(x, y, border, h);
	twod_drawQuad(x + w - border, y, border, h);
}

#endif
