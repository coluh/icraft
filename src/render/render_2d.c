#include "render_2d.h"
#include "gl.h"

#include "../../third_party/cglm/include/cglm/affine.h"
#include "../../third_party/cglm/include/cglm/mat4.h"
#include "../../third_party/cglm/include/cglm/types.h"

void twod_setColor(float r, float g, float b, float a) {
	gl_begin2d();
	gl_setUniform4f(true, "color", r, g, b, a);
}

void twod_drawQuad(int x, int y, int w, int h) {
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){x, y, 0.0f});
	glm_scale(model, (vec3){w, h, 1.0f});

	mat4 mvp;
	glm_mat4_mul(*gl_getProj2d(), model, mvp);

	gl_begin2d();
	gl_setUniformMatrix4fv(true, "proj", mvp);
	gl_renderQuad();
}
