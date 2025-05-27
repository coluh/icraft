#ifndef _ICRAFT_render_gl_h
#define _ICRAFT_render_gl_h

#include <stdbool.h>
#include <glad/glad.h>
#include "../util/log.h"

#define GL_CHECK(stmt) \
	do { \
		stmt; \
		GLenum err = glGetError(); \
		if (err != GL_NO_ERROR) { \
			if (err == GL_INVALID_ENUM) { \
				logw("OpenGL Error: Invalid Enum"); \
			} else if (err == GL_INVALID_VALUE) { \
				logw("OpenGL Error: Invalid Value"); \
			} else if (err == GL_INVALID_OPERATION) { \
				logw("OpenGL Error: Invalid Operation"); \
			} else if (err == GL_INVALID_FRAMEBUFFER_OPERATION) { \
				logw("OpenGL Error: Invalid Framebuffer Operation"); \
			} else { \
				logw("OpenGL Error: ..."); \
			} \
			loge("Exit. Check your OpenGL Error!"); \
		} \
	} while (0) \

void gl_init();
void gl_setSize(int width, int height);
void gl_clear(float r, float g, float b, float a);

#endif
