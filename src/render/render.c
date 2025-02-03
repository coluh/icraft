#include "render.h"
#include "shader.h"
#include "texture.h"
#include "window.h"
#include "../world/chunk.h"

#include <glad/glad.h>
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
	int viewUniformLocation;
	int projUniformLocation;
	unsigned int blocksTexture;

	int program2d;
	unsigned int VAO2d;
	unsigned int VBO2d;
	unsigned int EBO2d;
	SDL_Color color;
} r;

Chunk debug_chunk;

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
	r.viewUniformLocation = glGetUniformLocation(r.program, "view");
	r.projUniformLocation = glGetUniformLocation(r.program, "proj");
	r.program2d = shader_get("assets/shaders/basic2d.vs", "assets/shaders/basic2d.fs");

	render_setColor(255, 255, 255, 255);

	r.blocksTexture = texture_load("assets/textures/texture.png");

	chunk_debugFill(&debug_chunk);
}

int render_getFPS() { return r.FPS; }

static void render_chunks(Camera *camera);
void render(Camera *camera) {
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3D content
	camera_update(camera);
	render_chunks(camera);

	// 2D content
	render_setColor(127, 0, 255, 255);
	render_fill_rect(50, 50, 100, 100);

	SDL_GL_SwapWindow(window_getWindow());
}

static void render_chunks(Camera *camera) {
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(r.program);

	glUniformMatrix4fv(r.viewUniformLocation, 1, GL_FALSE, (float*)camera->view);
	glUniformMatrix4fv(r.projUniformLocation, 1, GL_FALSE, (float*)camera->proj);

	glActiveTexture(GL_TEXTURE0);  // no need, because it's default
	glBindTexture(GL_TEXTURE_2D, r.blocksTexture);
	glBindVertexArray(debug_chunk.VAO);
	glDrawArrays(GL_TRIANGLES, 0, debug_chunk.vertex_count);
}

static void render_setColor(int rr, int g, int b, int a) {
	r.color.r = rr;
	r.color.g = g;
	r.color.b = b;
	r.color.a = a;
}

static void render_fill_rect(int x, int y, int w, int h) {
	static float vertices[3*4];
	static unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };
	int wwd2 = window_getWidth();
	int whd2 = window_getHeight();
	float x1 = (float)x / wwd2 - 1.0f;
	float y1 = (float)y / whd2 - 1.0f;
	y1 = -y1;
	float x2 = (float)(x+w) / wwd2 - 1.0f;
	float y2 = (float)(y+h) / whd2 - 1.0f;
	y2 = -y2;
	vertices[0] = vertices[3] = x2;
	vertices[6] = vertices[9] = x1;
	vertices[1] = vertices[10] = y1;
	vertices[4] = vertices[7] = y2;
	glBindVertexArray(r.VAO2d);
	glBindBuffer(GL_ARRAY_BUFFER, r.VBO2d);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r.EBO2d);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(r.program2d);
	glUniform4f(glGetUniformLocation(r.program2d, "color"),
			r.color.r/256.0f, r.color.g/256.0f, r.color.b/256.0f, r.color.a/256.0f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
