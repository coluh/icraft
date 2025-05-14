#ifndef _ICRAFT_render_gl_h
#define _ICRAFT_render_gl_h

#include <stdbool.h>
#include <glad/glad.h>

void gl_init();
void gl_setSize(int width, int height);
void gl_clear(float r, float g, float b, float a);

#endif
