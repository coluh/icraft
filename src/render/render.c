#include "render.h"
#include "shader.h"
#include "texture.h"
#include "window.h"
#include "../world/chunk.h"

#include <glad/glad.h>
#include "../../third_party/cglm/include/cglm/cam.h"
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <SDL2/SDL.h>

static struct {
	SDL_Renderer *renderer;
	int FPS;
	int program;
	int modelUniformLocation;
	int viewUniformLocation;
	int projUniformLocation;
	unsigned int blocksTexture;

	int program2d;
	mat4 proj2d;
	unsigned int VAO2d;
	unsigned int VBO2d;
	unsigned int EBO2d;
	SDL_Color color;
} r;

static void render_setColor(int rr, int g, int b, int a);
static void render_fill_rect(int x, int y, int w, int h);

void render_init() {
	r.renderer = SDL_CreateRenderer(window_getWindow(), -1, 0);
	r.FPS = 60;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	// glEnableVertexAttribArray(0);

	r.VAO2d = VAO;
	r.VBO2d = VBO;
	r.EBO2d = EBO;
	r.program = shader_get("assets/shaders/default.vs", "assets/shaders/default.fs");
	r.modelUniformLocation = glGetUniformLocation(r.program, "model");
	r.viewUniformLocation = glGetUniformLocation(r.program, "view");
	r.projUniformLocation = glGetUniformLocation(r.program, "proj");
	r.program2d = shader_get("assets/shaders/basic2d.vs", "assets/shaders/basic2d.fs");
	glm_ortho(0.0f, window_getWidth(), window_getHeight(), 0.0f, -1.0f, 1.0f, r.proj2d);

	render_setColor(255, 255, 255, 255);

	r.blocksTexture = texture_load("assets/textures/texture.png");
}

int render_getFPS() { return r.FPS; }

static void render_chunks(Camera *camera);
void render(Camera *camera) {
	glClearColor(0.4f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3D content
	camera_update(camera);
	render_chunks(camera);

	// 2D content
	render_setColor(127, 0, 255, 255);
	render_fill_rect(50, 50, 100, 100);

	SDL_GL_SwapWindow(window_getWindow());
}

static void render_chunk(Chunk *chunk) {
	glUniformMatrix4fv(r.modelUniformLocation, 1, GL_FALSE, (float*)chunk->model);
	glBindVertexArray(chunk->VAO);
	glDrawArrays(GL_TRIANGLES, 0, chunk->vertex_count);
}

static void render_chunks(Camera *camera) {
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(r.program);
	glUniformMatrix4fv(r.viewUniformLocation, 1, GL_FALSE, (float*)camera->view);
	glUniformMatrix4fv(r.projUniformLocation, 1, GL_FALSE, (float*)camera->proj);

	glActiveTexture(GL_TEXTURE0);  // no need, because tex0 is activated by default
	glBindTexture(GL_TEXTURE_2D, r.blocksTexture);
	chunks_foreach(render_chunk);
}

static void render_setColor(int rr, int g, int b, int a) {
	r.color.r = rr;
	r.color.g = g;
	r.color.b = b;
	r.color.a = a;
}

static void render_fill_rect(int x, int y, int w, int h) {
	static float vertices[2*4];
	// x, y
	// x+w, y
	// x+w, y+h
	// x, y+h

	vertices[0] = vertices[6] = x;
	vertices[1] = vertices[3] = y;
	vertices[2] = vertices[4] = x + w;
	vertices[5] = vertices[7] = y + h;
	glBindVertexArray(r.VAO2d);
	glBindBuffer(GL_ARRAY_BUFFER, r.VBO2d);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(r.program2d);
	glUniformMatrix4fv(glGetUniformLocation(r.program2d, "proj"), 1, GL_FALSE, (float*)r.proj2d);
	glUniform4f(glGetUniformLocation(r.program2d, "color"),
			r.color.r/256.0f, r.color.g/256.0f, r.color.b/256.0f, r.color.a/256.0f);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
}
