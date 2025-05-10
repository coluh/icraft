#include "../element.h"
#include <SDL2/SDL_events.h>
#include <stdbool.h>

typedef struct uiSlider {
	uiElement e;
	char text[ELEMENT_MAX_TEXT_SIZE];
	bool focused;
	int min, max;
} uiSlider;

uiElement *ui_newElementSlider(const char *format, int min, int max) {
	return NULL;
}

void ui_updateElementSlider(uiElement *m, SDL_Event *e) {
	;
}

void ui_renderElementLabel(uiElement *m);

void ui_renderElementSlider(uiElement *m) {
	ui_renderElementLabel(m);
}
