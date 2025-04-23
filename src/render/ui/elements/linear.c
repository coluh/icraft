#include "../element.h"
#include "../../../util/mem.h"
#include "../../../util/log.h"
#include "../../../util/props.h"
#include <SDL2/SDL_events.h>
#include <stdbool.h>

typedef struct uiLinear {
	uiElement e;
	bool computed;
	bool horizontal;
	int count;
	uiElement *children[];
} uiLinear;

static int padding = ELEMENT_PADDING;

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

void ui_arrangeLinearLayout(uiElement *m, bool stretch) {
	uiLinear *l = (uiLinear*)m;

	if (l->count == 0) {
		l->computed = true;
		return;
	}

	if (!l->horizontal) {
		// let padding = avgh/4, which means nx + (n-1)x/4 = h, that avgh = x = 4h/(5n-1)
		int avgh = m->rect.h * 4 / (5 * l->count - 1);
		padding = avgh / 4;
	}

	int avgw = (m->rect.w - padding * (l->count - 1)) / l->count;
	int avgh = (m->rect.h - padding * (l->count - 1)) / l->count;
	if (l->horizontal) {
		for (int i = 0; i < l->count; i++) {
			l->children[i]->rect.w = avgw;
			l->children[i]->rect.h = m->rect.h;
		}
	} else {
		for (int i = 0; i < l->count; i++) {
			l->children[i]->rect.w = m->rect.w;
			l->children[i]->rect.h = avgh;
		}
	}

	int x = m->rect.x;
	int y = m->rect.y;
	FORR(l->count) {
		uiElement *c = l->children[i];
		if (l->horizontal) {
			c->rect.x = x;
			c->rect.y = y;
		} else {
			c->rect.x = x;
			c->rect.y = y;
		}
		if (c->type == Element_Linear) {
			ui_arrangeLinearLayout(c, stretch);
		}
		if (l->horizontal) {
			x += c->rect.w + padding;
		} else {
			y += c->rect.h + padding;
		}
	}

	l->computed = true;
}
