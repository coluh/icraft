#ifndef _ICRAFT_render_font_h
#define _ICRAFT_render_font_h

void font_init(const char *font_path);
void font_drawText(const char *utf8str, float x, float y, float scale);

#endif
