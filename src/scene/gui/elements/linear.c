#include "../element.h"
#include "../../../util/mem.h"
#include "../../../util/log.h"
#include "../../../util/props.h"
#include <SDL2/SDL_events.h>
#include <stdbool.h>
#include "../../../game.h"

extern Game g;

typedef struct uiLinear {
	uiElement e;
	bool computed;
	bool horizontal;
	int count;
	uiElement *children[];
} uiLinear;

uiElement *ui_newElementLinear(uiElement *children[], int count, bool horizontal) {
	uiLinear *l = zalloc(1, sizeof(uiLinear) + count * sizeof(uiElement*));
	ui_assignElementId(&l->e);

	l->e.type = Element_Linear;
	l->computed = false;
	l->horizontal = horizontal;
	l->count = count;
	for (int i = 0; i < count; i++) {
		l->children[i] = children[i];
	}

	return (uiElement*)l;
}

void ui_updateElementLinear(uiElement *m, SDL_Event *e) {
	uiLinear *l = (uiLinear*)m;

	for (int i = 0; i < l->count; i++) {
		ui_updateElement(l->children[i], e);
	}
}

void ui_renderElementLinear(uiElement *m) {
	uiLinear *l = (uiLinear*)m;
	if (!l->computed) {
		loge("Linear layout not computed");
	}

	for (int i = 0; i < l->count; i++) {
		ui_renderElement(l->children[i]);
	}
}

static void computeSize(uiElement *m) {
	uiLinear *l = (uiLinear*)m;

	Assert(m->rect.w != 0, "linear width not set");
	if (l->horizontal) {
		int average_width = (m->rect.w - (g.zoom_level * 5 * (l->count - 1))) / l->count;
		for (int i = 0; i < l->count; i++) {
			l->children[i]->rect.w = average_width;
			l->children[i]->rect.h = m->rect.h;
		}
	} else {
		for (int i = 0; i < l->count; i++) {
			l->children[i]->rect.w = m->rect.w;
			l->children[i]->rect.h = g.zoom_level * 20;
		}
		m->rect.h = SUM_FIELD_INT_P(l->children, uiElement, rect.h, l->count);
		m->rect.h += g.zoom_level * 5 * (l->count - 1);
	}
	for (int i = 0; i < l->count; i++) {
		if (l->children[i]->type == Element_Linear) {
			computeSize(l->children[i]);
		}
	}
}

void ui_arrangeLinearLayout(uiElement *m) {
	uiLinear *l = (uiLinear*)m;

	if (l->count == 0) {
		l->computed = true;
		return;
	}

	computeSize(m);

	int x = m->rect.x;
	int y = m->rect.y;
	FORR(l->count) {
		uiElement *c = l->children[i];
		c->rect.x = x;
		c->rect.y = y;
		if (c->type == Element_Linear) {
			ui_arrangeLinearLayout(c);
		}
		if (l->horizontal) {
			x += c->rect.w + g.zoom_level * 5;
		} else {
			y += c->rect.h + g.zoom_level * 5;
		}
	}

	l->computed = true;
}
