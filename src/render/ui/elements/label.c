#include "../element.h"
#include "../../render_2d.h"
#include "../../font.h"
// #include "../../../util/props.h"
#include "../../../util/mem.h"
#include <SDL2/SDL_events.h>

typedef struct uiLabel {
	uiElement e;
	const char *text;
} uiLabel;

uiElement *ui_newElementLabel(const char *text) {
	uiLabel *l = zalloc(1, sizeof(uiLabel));
	ui_assignElementId(&l->e);

	l->e.type = Element_Label;
	l->text = text;
	// debug, should be min size here
	l->e.rect.w = 400;
	l->e.rect.h = 50;
	return (uiElement*)l;
}

void ui_updateElementLabel(uiElement *m, SDL_Event *e) {}

void ui_renderElementLabel(uiElement *m) {
	const uiLabel *l = (uiLabel*)m;

	twod_setColor(ELEMENT_FONT_COLOR, 1.0f);
	font_drawText(l->text, m->rect.x, m->rect.y + (float)m->rect.h*2/3, 1);
	// twod_drawQuad(UNPACK_RECT_SHRINK(l->e.rect, l->e.rect.h/4));
}
