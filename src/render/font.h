#ifndef _ICRAFT_render_font_h
#define _ICRAFT_render_font_h

void font_init(const char *font_path);
void font_queryText(const char *utf8str, float scale, int *width, int *height);

// draw text given left-top coord
void font_drawText(const char *utf8str, float x, float y, float scale);
// draw text given center coord
void font_drawTextCentered(const char *utf8str, float x, float y, float scale);

#endif
