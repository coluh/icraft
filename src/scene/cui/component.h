#ifndef _ICRAFT_scene_cui_component_h
#define _ICRAFT_scene_cui_component_h

#include "../../render/render_2d.h"
#include "../../item/item.h"
#include <stdio.h>
#include "../../render/font.h"
#include "../../util/props.h"

static inline void outline(int x, int y, int w, int h, int border) {
	twod_setColor(0.95, 0.95, 0.95, 1);
	twod_drawQuad(x - border/2, y - border, w + border/2, border);
	twod_drawQuad(x - border, y - border/2, border, h + border/2);
	twod_setColor(0.2, 0.2, 0.2, 1);
	twod_drawQuad(x + w, y, border, h + border);
	twod_drawQuad(x, y + h, w + border, border);
}

static inline void grayquad(int x, int y, int w, int h, float gray) {
	twod_setColor(gray, gray, gray, 1.0f);
	twod_drawQuad(x, y, w, h);
}

static inline void draw_slot(int x, int y, int w, int h, int shadow) {
	grayquad(x, y, w, h, 0.5f);
	twod_setColor(0.95, 0.95, 0.95, 1.0);
	twod_drawQuad(x + w - shadow, y, shadow, h);
	twod_drawQuad(x, y + h - shadow, w, shadow);
	twod_setColor(0, 0, 0, 0.5);
	twod_drawQuad(x, y, w, shadow);
	twod_drawQuad(x, y, shadow, h);
}

static inline void draw_item(const Slot *slot, int x, int y, int a, int zoom_level) {
	if (slot->count > 0) {
		const int t = zoom_level;
		if (item_is3d(slot->item.id)) {
			twod_drawTexture(x, y, a, a, item_3dIconTexture(slot->item.id));
		} else {
			twod_drawIndexedTexture(x + t, y + t, a - 2*t, a - 2*t, item_textureIndex(slot->item.id));
		}
		if (slot->count > 1) {
			char count[4];
			if (slot->count < 100) {
				snprintf(count, 3, "%2d", slot->count);
			} else {
				snprintf(count, 4, "99+");
			}
			twod_setColor(0.0f, 0.0f, 0.0f, 1.0f);
			font_drawTextCentered(count, x+a*0.7+2, y+a*0.8+2, LEVEL_CHOOSE(zoom_level, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0));
			twod_setColor(1.0f, 1.0f, 1.0f, 1.0f);
			font_drawTextCentered(count, x+a*0.7, y+a*0.8, LEVEL_CHOOSE(zoom_level, 0.5, 0.75, 1.0, 1.5, 2.0, 2.5, 3.0));
		}
	}
}

#endif
