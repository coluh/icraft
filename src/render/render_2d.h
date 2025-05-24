#ifndef _ICRAFT_render_render2d_h
#define _ICRAFT_render_render2d_h

#include <glad/glad.h>

void twod_setColor(float r, float g, float b, float a);
void twod_drawQuad(int x, int y, int w, int h);
void twod_drawTextureShape(int x, int y, int w, int h, GLuint texture);
void twod_drawTexture(int x, int y, int w, int h, GLuint texture);
void twod_drawIndexedTexture(int x, int y, int w, int h, unsigned int id);

#endif
