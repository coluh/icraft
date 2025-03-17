#ifndef _ICRAFT_render_gl_h
#define _ICRAFT_render_gl_h

#include <stdbool.h>
#include "../../third_party/cglm/include/cglm/types.h"

void gl_init();

mat4 *gl_getProj2d();
void gl_begin2d();
void gl_renderQuad();

void gl_begin3d();
void gl_setView(mat4 view);
void gl_setProj(mat4 proj);
void gl_setModel(mat4 model);
void gl_renderTriangles(int vao, int count);

void gl_clear(float r, float g, float b, float a);
void gl_setUniform4f(bool program2d, const char *name, float v0, float v1, float v2, float v3);
void gl_setUniformMatrix4fv(bool program2d, const char *name, mat4 matrix);

void gl_bindBlocksTexture();

#endif
