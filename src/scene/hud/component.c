#include "component.h"
#include "../../render/render_2d.h"
 
void bar(int x, int y, int w, int h, int border) {
	twod_drawQuad(x, y, w, border);
	twod_drawQuad(x, y + h - border, w, border);
	twod_drawQuad(x, y, border, h);
	twod_drawQuad(x + w - border, y, border, h);
}
