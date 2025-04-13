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

void ui_arrangeLinearLayout(uiElement *m) {
	uiLinear *l = (uiLinear*)m;

	// debug
	if (l->horizontal) {
		for (int i = 0; i < l->count; i++) {
			l->children[i]->rect.x = l->e.rect.x + 200 * i;
			l->children[i]->rect.y = l->e.rect.y;
		}
	} else {
		for (int i = 0; i < l->count; i++) {
			l->children[i]->rect.x = l->e.rect.x;
			l->children[i]->rect.y = l->e.rect.y + 50 * i;
		}
	}

	for (int i = 0; i < l->count; i++) {
		uiElement *e = l->children[i];
		if (e->type == Element_Linear) {
			ui_arrangeLinearLayout(e);
		}
	}

	l->computed = true;
}
