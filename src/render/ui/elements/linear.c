#include "../element.h"
#include "../../../util/mem.h"
#include "../../../util/log.h"
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
	l->horizontal = horizontal;
	l->computed = false;
	l->count = count;
	l->e.rect.w = ELEMENT_PADDING * (count - 1);
	l->e.rect.h = ELEMENT_PADDING * (count - 1);
	for (int i = 0; i < count; i++) {
		l->children[i] = children[i];
		l->e.rect.w += children[i]->rect.w;
		l->e.rect.h += children[i]->rect.h;
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

	for (int i = 0; i < l->count; i++) {
		uiElement *e = l->children[i];
		if (l->horizontal) {
			e->rect.x = m->rect.x + 220 * i;
			e->rect.y = m->rect.y;
		} else {
			e->rect.x = m->rect.x;
			e->rect.y = m->rect.y + 70 * i;
		}
		if (e->type == Element_Linear) {
			ui_arrangeLinearLayout(e);
		}
	}

		// logd("{ %d, %d, %d, %d }", e->rect.x, e->rect.y, e->rect.w, e->rect.h);

	l->computed = true;
}
