#include "gl.h"
#include "window.h"
#include "shader.h"
#include "texture.h"
#include "../util/props.h"

#include <glad/glad.h>
#include "../../third_party/cglm/include/cglm/cam.h"
#include "../../third_party/cglm/include/cglm/types.h"
#include <SDL2/SDL.h>

static struct {
	int program;
	int modelUniformLocation;
	int viewUniformLocation;
	int projUniformLocation;
	unsigned int blocksTexture;

	int program2d;
	mat4 proj2d;
	unsigned int VAO2d;
	unsigned int VBO2d;
	// unsigned int EBO2d;
	struct { float r, g, b, a; } color2d;
} renderer;

void gl_init() {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	renderer.program = shader_get("assets/shaders/default.vs", "assets/shaders/default.fs");
	renderer.modelUniformLocation = glGetUniformLocation(renderer.program, "model");
	renderer.viewUniformLocation = glGetUniformLocation(renderer.program, "view");
	renderer.projUniformLocation = glGetUniformLocation(renderer.program, "proj");
	renderer.blocksTexture = texture_load("assets/textures/texture.png");

	renderer.program2d = shader_get("assets/shaders/basic2d.vs", "assets/shaders/basic2d.fs");
	glm_ortho(0.0f, window_getWidth(), window_getHeight(), 0.0f, -1.0f, 1.0f, renderer.proj2d);

	float vertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	renderer.VAO2d = VAO;
	renderer.VBO2d = VBO;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	PACK_RGBA(renderer.color2d, 1.0f, 1.0f, 1.0f, 1.0f);
}

mat4 *gl_getProj2d() {
	return &renderer.proj2d;
}

void gl_setView(mat4 view) {
	glUniformMatrix4fv(renderer.viewUniformLocation, 1, GL_FALSE, (float*)view);
}

void gl_setProj(mat4 proj) {
	glUniformMatrix4fv(renderer.projUniformLocation, 1, GL_FALSE, (float*)proj);
}

void gl_setModel(mat4 model) {
	glUniformMatrix4fv(renderer.modelUniformLocation, 1, GL_FALSE, (float*)model);
}

void gl_clear(float r, float g, float b, float a) {
	glClearColor(0.4f, 0.8f, 1.0f, 1.0f); // good color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gl_setUniform4f(bool program2d, const char *name, float v0, float v1, float v2, float v3) {
	int program = program2d ? renderer.program2d : renderer.program;
	glUniform4f(glGetUniformLocation(program, "color"), v0, v1, v2, v3);
}

void gl_setUniformMatrix4fv(bool program2d, const char *name, mat4 matrix) {
	int program = program2d ? renderer.program2d : renderer.program;
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, (float*)matrix);
}

void gl_begin2d() {
	glUseProgram(renderer.program2d);
}

void gl_begin3d() {
	glUseProgram(renderer.program);
}

void gl_bindBlocksTexture() {
	glActiveTexture(GL_TEXTURE0);  // no need, because tex0 is activated by default
	glBindTexture(GL_TEXTURE_2D, renderer.blocksTexture);
}

void gl_renderQuad() {
	glBindVertexArray(renderer.VAO2d);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
}

void gl_renderTriangles(int vao, int count) {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, count);
}
