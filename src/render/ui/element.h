#ifndef _ICRAFT_render_ui_element_h
#define _ICRAFT_render_ui_element_h

#include <stdbool.h>
#include <SDL2/SDL_events.h>

typedef enum uiElementType {
	Element_Label, Element_Button, Element_Slider,
	Element_Linear,
} uiElementType;

typedef struct uiElement {
	int id; // element id is unique in one scene
	uiElementType type;
	struct {
		int x, y, w, h;
	} rect;
} uiElement;

#define ELEMENT_BORDER_COLOR_UNFOCUS 0.0f,0.0f,0.0f
#define ELEMENT_BORDER_COLOR_FOCUSED 1.0f,1.0f,1.0f
#define ELEMENT_BORDER_COLOR_UPLEFT 0.66f,0.66f,0.66f
#define ELEMENT_BORDER_COLOR_BOTTOMRIGHT 0.33f,0.33f,0.33f
#define ELEMENT_BACKGROUND_COLOR 0.44f,0.44f,0.44f
#define ELEMENT_BORDER_THICKNESS 5
#define ELEMENT_PADDING 20

#define ELEMENT_FONT_COLOR 0.0f,0.0f,0.0f

#define ELEMENT_MAX_TEXT_SIZE 64

void ui_assignElementId(uiElement *e);

void ui_updateElement(uiElement *m, SDL_Event *e);
void ui_renderElement(uiElement *m);

uiElement *ui_newElementButton(const char *text, void (*callback)());
uiElement *ui_newElementLabel(const char *text);
uiElement *ui_newElementLinear(uiElement *children[], int count, bool horizontal);
// uiElement *ui_newElementSlider(const char *format, int min, int max) {

// you must set linear's xywh before calling this
void ui_arrangeLinearLayout(uiElement *m);

#endif
