#include "../element.h"
#include "../../../render/render_2d.h"
#include "../../../render/font.h"
#include "../../../util/props.h"
#include "../../../util/mem.h"
#include <SDL2/SDL_events.h>

typedef struct uiLabel {
	uiElement e;
	const char *text;
	int text_width, text_height;
} uiLabel;

uiElement *ui_newElementLabel(const char *text) {
	uiLabel *l = zalloc(1, sizeof(uiLabel));
	ui_assignElementId(&l->e);

	l->e.type = Element_Label;
	l->text = text;
	font_queryText(l->text, 1, &l->text_width, &l->text_height);
	return (uiElement*)l;
}

void ui_updateElementLabel(uiElement *m, SDL_Event *e) {}

void ui_renderElementLabel(uiElement *m) {
	const uiLabel *l = (uiLabel*)m;

	twod_setColor(ELEMENT_FONT_COLOR, 1.0f);
	font_drawTextCentered(l->text,
		m->rect.x + m->rect.w / 2.0f,
		m->rect.y + m->rect.h / 2.0f,
	LEVEL_CHOOSE(g.zoom_level, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0));
}
