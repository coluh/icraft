#include "render_2d.h"
#include "../game.h"
#include "resource.h"
#include "texture.h"

#include "../../third_party/cglm/include/cglm/affine.h"
#include "../../third_party/cglm/include/cglm/mat4.h"
#include "../../third_party/cglm/include/cglm/types.h"

extern Game g;

void twod_setColor(float r, float gg, float b, float a) {
	glUseProgram(g.res->shaders.ui);
	glUniform4f(g.res->shaders.ui_location.color, r, gg, b, a);
}

static void twod_setRect(int x, int y, int w, int h) {
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){x, y, 0.0f});
	glm_scale(model, (vec3){w, h, 1.0f});

	glUseProgram(g.res->shaders.ui);
	glUniformMatrix4fv(g.res->shaders.ui_location.model, 1, GL_FALSE, (float*)model);
}

void twod_drawQuad(int x, int y, int w, int h) {
	twod_setRect(x, y, w, h);
	glBindVertexArray(g.res->meshes.rectangleVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
}

void twod_drawTextureShape(int x, int y, int w, int h, GLuint texture) {
	twod_setRect(x, y, w, h);

	glUniform1i(g.res->shaders.ui_location.useTextureAlpha, 1);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(g.res->meshes.rectangleVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(g.res->shaders.ui_location.useTextureAlpha, 0);
}

void twod_drawTexture(int x, int y, int w, int h, GLuint texture) {
	twod_setRect(x, y, w, h);

	glUniform1i(g.res->shaders.ui_location.useTexture, 1);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(g.res->meshes.rectangleVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(g.res->shaders.ui_location.useTexture, 0);
}

void twod_drawIndexedTexture(int x, int y, int w, int h, int index) {
	twod_setRect(x, y + h, w, -h);

	glUniform1i(g.res->shaders.ui_location.useTexture, 1);
	glBindTexture(GL_TEXTURE_2D, g.res->textures.blocks);
	glUniform1i(g.res->shaders.ui_location.use_uv_offset, 1);
	float uv[2];
	texture_blockUVoffset(index, uv);
	glUniform2f(g.res->shaders.ui_location.uv_offset, uv[0], uv[1]);

	glBindVertexArray(g.res->meshes.rectangleVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);

	glUniform1i(g.res->shaders.ui_location.use_uv_offset, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(g.res->shaders.ui_location.useTexture, 0);
}
