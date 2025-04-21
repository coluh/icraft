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

	if (l->count > 0) {
		// set w, h
		l->e.rect.w = 0;
		l->e.rect.h = 0;
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

	int totalWidth = SUM_FIELD_INT_P(l->children, uiElement, rect.w, l->count);
	int totalHeight = SUM_FIELD_INT_P(l->children, uiElement, rect.h, l->count);
	if (m->rect.w == 0) {
		if (l->horizontal) {
			m->rect.w = totalWidth + ELEMENT_PADDING * (l->count - 1);
		} else {
			m->rect.w = MAX_FIELD_INT_P(l->children, uiElement, rect.w, l->count);
		}
	}
	if (m->rect.h == 0) {
		if (l->horizontal) {
			m->rect.h = MAX_FIELD_INT_P(l->children, uiElement, rect.h, l->count);
		} else {
			m->rect.h = totalHeight + ELEMENT_PADDING * (l->count - 1);
		}
	}

	if (!l->horizontal) {
		// stretch them
		for (int i = 0; i < l->count; i++) {
			l->children[i]->rect.w = m->rect.w;
		}
	}

	int x = m->rect.x;
	int y = m->rect.y;
	int paddingW = (m->rect.w - totalWidth) / (l->count - 1);
	int paddingH = (m->rect.h - totalHeight) / (l->count - 1);
	if (!stretch) {
		paddingW = ELEMENT_PADDING;
		paddingH = ELEMENT_PADDING;
	}
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
			x += c->rect.w + paddingW;
		} else {
			y += c->rect.h + paddingH;
		}
	}

	l->computed = true;
}
