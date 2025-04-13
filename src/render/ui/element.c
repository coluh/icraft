#include "element.h"

static int elementId = 0;

void ui_assignElementId(uiElement *e) {
	e->id = elementId;
	elementId++;
}

void ui_updateElementLabel(uiElement *m, SDL_Event *e);
void ui_updateElementButton(uiElement *m, SDL_Event *e);
void ui_updateElementSlider(uiElement *m, SDL_Event *e);
void ui_updateElementLinear(uiElement *m, SDL_Event *e);

void ui_updateElement(uiElement *m, SDL_Event *e) {
	switch (m->type) {
	case Element_Label:
		ui_updateElementLabel(m, e);
		break;
	case Element_Button:
		ui_updateElementButton(m, e);
		break;
	case Element_Slider:
		ui_updateElementSlider(m, e);
		break;
	case Element_Linear:
		ui_updateElementLinear(m, e);
	default:
		break;
	}
}

void ui_renderElementLabel(uiElement *m);
void ui_renderElementButton(uiElement *m);
void ui_renderElementSlider(uiElement *m);
void ui_renderElementLinear(uiElement *m);

void ui_renderElement(uiElement *m) {
	switch (m->type) {
	case Element_Label:
		ui_renderElementLabel(m);
		break;
	case Element_Button:
		ui_renderElementButton(m);
		break;
	case Element_Slider:
		ui_renderElementSlider(m);
		break;
	case Element_Linear:
		ui_renderElementLinear(m);
		break;
	default:
		break;
	}
}
