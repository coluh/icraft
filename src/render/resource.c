#include "resource.h"
#include "../game.h"
#include "../util/mem.h"
#include <glad/glad.h>
#include <string.h>
#include "shader.h"
#include "texture.h"
#include "../world/block/block.h"

extern Game g;

void resource_init() {
	ResourceManager *res = zalloc(1, sizeof(ResourceManager));

	res->shaders.basic = shader_get("assets/shaders/default.vs", "assets/shaders/default.fs");
	res->shaders.basic_location.model = glGetUniformLocation(res->shaders.basic, "model");
	res->shaders.basic_location.view = glGetUniformLocation(res->shaders.basic, "view");
	res->shaders.basic_location.proj = glGetUniformLocation(res->shaders.basic, "proj");
	res->shaders.basic_location.normal_matrix = glGetUniformLocation(res->shaders.basic, "normal_matrix");
	res->shaders.basic_location.use_uv_offset = glGetUniformLocation(res->shaders.basic, "use_uv_offset");
	res->shaders.basic_location.uv_offset = glGetUniformLocation(res->shaders.basic, "uv_offset");
	res->shaders.ui = shader_get("assets/shaders/basic2d.vs", "assets/shaders/basic2d.fs");
	res->shaders.ui_location.model = glGetUniformLocation(res->shaders.ui, "model");
	res->shaders.ui_location.proj = glGetUniformLocation(res->shaders.ui, "proj");
	res->shaders.ui_location.color = glGetUniformLocation(res->shaders.ui, "color");
	res->shaders.ui_location.useTexture = glGetUniformLocation(res->shaders.ui, "useTexture");

	res->textures.blocks = texture_load("assets/textures/texture.png");
	// for (int i = 0; i < 10; i++) {
	// 	char filename[] = "assets/textures/destroy_stage_#.png";
	// 	*strchr(filename, '#') = '0' + i;
	// 	res->textures.destroy_stages[i] = texture_load(filename);
	// }

	float rectangle_vertices[] = {
		// pos      // tex
		0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
	};
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	res->meshes.rectangleVAO = VAO;
	res->meshes.rectangleVAO_count = 4;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// cube VAO
	float cube_faces_vertices[][8] = {
			{1, 0, 0, 0, 0, -1, 0, 0}, {0, 0, 0, 0, 0, -1, 1, 0}, {0, 1, 0, 0, 0, -1, 1, 1},
			{1, 0, 0, 0, 0, -1, 0, 0}, {0, 1, 0, 0, 0, -1, 1, 1}, {1, 1, 0, 0, 0, -1, 0, 1},
			{0, 0, 1, 0, 0, 1, 0, 0}, {1, 0, 1, 0, 0, 1, 1, 0}, {1, 1, 1, 0, 0, 1, 1, 1},
			{0, 0, 1, 0, 0, 1, 0, 0}, {1, 1, 1, 0, 0, 1, 1, 1}, {0, 1, 1, 0, 0, 1, 0, 1},
			{0, 0, 0, 0, -1, 0, 0, 0}, {1, 0, 0, 0, -1, 0, 1, 0}, {1, 0, 1, 0, -1, 0, 1, 1},
			{0, 0, 0, 0, -1, 0, 0, 0}, {1, 0, 1, 0, -1, 0, 1, 1}, {0, 0, 1, 0, -1, 0, 0, 1},
			{0, 1, 0, 0, 1, 0, 0, 0}, {0, 1, 1, 0, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 0, 1, 1},
			{0, 1, 0, 0, 1, 0, 0, 0}, {1, 1, 1, 0, 1, 0, 1, 1}, {1, 1, 0, 0, 1, 0, 0, 1},
			{0, 0, 0, -1, 0, 0, 0, 0}, {0, 0, 1, -1, 0, 0, 1, 0}, {0, 1, 1, -1, 0, 0, 1, 1},
			{0, 0, 0, -1, 0, 0, 0, 0}, {0, 1, 1, -1, 0, 0, 1, 1}, {0, 1, 0, -1, 0, 0, 0, 1},
			{1, 0, 1, 1, 0, 0, 0, 0}, {1, 0, 0, 1, 0, 0, 1, 0}, {1, 1, 0, 1, 0, 0, 1, 1},
			{1, 0, 1, 1, 0, 0, 0, 0}, {1, 1, 0, 1, 0, 0, 1, 1}, {1, 1, 1, 1, 0, 0, 0, 1},
	};
	for (int i = 0; i < 36; i++) {
		cube_faces_vertices[i][0] -= 0.5;
		cube_faces_vertices[i][1] -= 0.5;
		cube_faces_vertices[i][2] -= 0.5;
		cube_faces_vertices[i][6] /= BLOCK_TEXTURE_ROW_COUNT;
		cube_faces_vertices[i][7] /= BLOCK_TEXTURE_ROW_COUNT;
	}

	glGenVertexArrays(1, &VAO);
	res->meshes.cubeVAO = VAO;
	res->meshes.cubeVAO_count = sizeof(cube_faces_vertices) / (8 * sizeof(float));
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_faces_vertices), cube_faces_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	g.res = res;
}
