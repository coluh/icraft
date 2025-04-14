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

	// set w, h
	if (l->horizontal) {
		l->e.rect.w = SUM_FIELD_INT_P(l->children, uiElement, rect.w, count) + ELEMENT_PADDING * (count - 1);
		l->e.rect.h = MAX_FIELD_INT_P(l->children, uiElement, rect.h, count);
	} else {
		l->e.rect.w = MAX_FIELD_INT_P(l->children, uiElement, rect.w, count);
		l->e.rect.h = SUM_FIELD_INT_P(l->children, uiElement, rect.h, count) + ELEMENT_PADDING * (count - 1);

		// stretch them
		for (int i = 0; i < count; i++) {
			l->children[i]->rect.w = l->e.rect.w;
		}
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

void ui_arrangeLinearLayout(uiElement *m) {
	uiLinear *l = (uiLinear*)m;

	int x = m->rect.x;
	int y = m->rect.y;
	for (int i = 0; i < l->count; i++) {
		uiElement *e = l->children[i];
		if (l->horizontal) {
			e->rect.x = x;
			e->rect.y = y;
			x += e->rect.w + ELEMENT_PADDING;
		} else {
			e->rect.x = x;
			e->rect.y = y;
			y += e->rect.h + ELEMENT_PADDING;
		}
		if (e->type == Element_Linear) {
			ui_arrangeLinearLayout(e);
		}
	}

		// logd("{ %d, %d, %d, %d }", e->rect.x, e->rect.y, e->rect.w, e->rect.h);

	l->computed = true;
}
