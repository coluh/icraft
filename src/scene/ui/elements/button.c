#include "../element.h"
#include "../../../render/render_2d.h"
#include "../../../render/font.h"
#include "../../../util/props.h"
#include "../../../util/mem.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <stdbool.h>

typedef struct uiButton {
	uiElement e;
	const char *text;
	bool focused;
	void (*callback)();
	int text_width, text_height;
} uiButton;

uiElement *ui_newElementButton(const char *text, void (*callback)()) {
	uiButton *b = zalloc(1, sizeof(uiButton));
	ui_assignElementId(&b->e);

	b->e.type = Element_Button;
	b->text = text;
	font_queryText(b->text, 1, &b->text_width, &b->text_height);
	b->callback = callback;
	b->focused = false;
	return (uiElement*)b;
}

void ui_updateElementButton(uiElement *m, SDL_Event *e) {
	uiButton *b = (uiButton*)m;

	if (e == NULL) {
		return;
	}

	switch (e->type) {
	case SDL_MOUSEMOTION:
		if (IN_RECT(e->button.x, e->button.y, b->e.rect)) {
			b->focused = true;
		} else {
			b->focused = false;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (b->focused && e->button.button == SDL_BUTTON_LEFT) {
			b->callback();
		}
		break;
	default:
		break;
	}
}

void ui_renderElementButton(uiElement *m) {
	const uiButton *b = (uiButton*)m;

	// outline
	if (b->focused) {
		twod_setColor(ELEMENT_BORDER_COLOR_FOCUSED, 1.0f);
	} else {
		twod_setColor(ELEMENT_BORDER_COLOR_UNFOCUS, 1.0f);
	}
	twod_drawQuad(UNPACK_RECT(b->e.rect));

	const int t = ELEMENT_BORDER_THICKNESS;
	const int x = b->e.rect.x;
	const int y = b->e.rect.y;
	const int w = b->e.rect.w;
	const int h = b->e.rect.h;

	// background
	twod_setColor(ELEMENT_BACKGROUND_COLOR, 1.0f);
	twod_drawQuad(x+t, y+t, w-2*t, h-2*t);

	// border
	twod_setColor(ELEMENT_BORDER_COLOR_UPLEFT, 1.0f);
	twod_drawQuad(x+t, y+t, w-2*t, t);
	twod_drawQuad(x+t, y+t, t, h-2*t);
	twod_setColor(ELEMENT_BORDER_COLOR_BOTTOMRIGHT, 0.5f);
	twod_drawQuad(x+t, y+h-3*t, w-2*t, 2*t);
	twod_drawQuad(x+w-2*t, y+t, t, h-2*t);

	// word
	twod_setColor(ELEMENT_FONT_COLOR, 1.0f);
	font_drawText(b->text,
		x + (w - b->text_width) / 2.0f,
		y + (h - b->text_height) / 2.0f,
	1);
}
