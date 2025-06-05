#include "resource.h"
#include "../game.h"
#include "../util/mem.h"
#include <glad/glad.h>
#include <string.h>
#include "shader.h"
#include "texture.h"
#include "../world/block.h"
#include "../item/item.h"

#include "../../third_party/cglm/include/cglm/cglm.h"

#define BLOCK_ICON_SIZE 64

extern Game g;

void resource_init() {
	ResourceManager *res = zalloc(1, sizeof(ResourceManager));
	g.res = res;

	// load shaders and store uniform locations
	res->shaders.basic = shader_get("assets/shaders/default.vs", "assets/shaders/default.fs");
	res->shaders.basic_location.model = glGetUniformLocation(res->shaders.basic, "model");
	res->shaders.basic_location.view = glGetUniformLocation(res->shaders.basic, "view");
	res->shaders.basic_location.proj = glGetUniformLocation(res->shaders.basic, "proj");
	res->shaders.basic_location.light = glGetUniformLocation(res->shaders.basic, "light");
	res->shaders.basic_location.normal_matrix = glGetUniformLocation(res->shaders.basic, "normal_matrix");
	res->shaders.basic_location.use_uv_offset = glGetUniformLocation(res->shaders.basic, "use_uv_offset");
	res->shaders.basic_location.uv_offset = glGetUniformLocation(res->shaders.basic, "uv_offset");
	res->shaders.basic_location.gamma_correct = glGetUniformLocation(res->shaders.basic, "gamma_correct");
	res->shaders.ui = shader_get("assets/shaders/basic2d.vs", "assets/shaders/basic2d.fs");
	res->shaders.ui_location.model = glGetUniformLocation(res->shaders.ui, "model");
	res->shaders.ui_location.proj = glGetUniformLocation(res->shaders.ui, "proj");
	res->shaders.ui_location.color = glGetUniformLocation(res->shaders.ui, "color");
	res->shaders.ui_location.useTexture = glGetUniformLocation(res->shaders.ui, "useTexture");
	res->shaders.ui_location.useTextureAlpha = glGetUniformLocation(res->shaders.ui, "useTextureAlpha");
	res->shaders.ui_location.use_uv_offset = glGetUniformLocation(res->shaders.ui, "use_uv_offset");
	res->shaders.ui_location.uv_offset = glGetUniformLocation(res->shaders.ui, "uv_offset");
	res->shaders.particle = shader_get("assets/shaders/particle.vs", "assets/shaders/particle.fs");
	res->shaders.particle_location.particlePos = glGetUniformLocation(res->shaders.particle, "particlePos");
	res->shaders.particle_location.view = glGetUniformLocation(res->shaders.particle, "view");
	res->shaders.particle_location.proj = glGetUniformLocation(res->shaders.particle, "proj");
	res->shaders.particle_location.size = glGetUniformLocation(res->shaders.particle, "size");
	res->shaders.particle_location.uv_offset = glGetUniformLocation(res->shaders.particle, "uv_offset");

	// load texture
	res->textures.blocks = texture_load("assets/textures/texture.png");
	// for (int i = 0; i < 10; i++) {
	// 	char filename[] = "assets/textures/destroy_stage_#.png";
	// 	*strchr(filename, '#') = '0' + i;
	// 	res->textures.destroy_stages[i] = texture_load(filename);
	// }

	// prepare rectangle mesh
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

	// prepare cube mesh
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

	// frame mesh
	float frame_vertices[][3] = {
		{1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 1, 0}, {0, 1, 0}, {1, 1, 0}, {1, 1, 0}, {1, 0, 0},
		{0, 0, 1}, {1, 0, 1}, {1, 0, 1}, {1, 1, 1}, {1, 1, 1}, {0, 1, 1}, {0, 1, 1}, {0, 0, 1},
		{0, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 1}, {1, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0},
		{0, 1, 0}, {0, 1, 1}, {0, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 0}, {1, 1, 0}, {0, 1, 0},
		{0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 1, 1}, {0, 1, 1}, {0, 1, 0}, {0, 1, 0}, {0, 0, 0},
		{1, 0, 1}, {1, 0, 0}, {1, 0, 0}, {1, 1, 0}, {1, 1, 0}, {1, 1, 1}, {1, 1, 1}, {1, 0, 1},
	};
	glGenVertexArrays(1, &VAO);
	res->meshes.frameVAO = VAO;
	res->meshes.frameVAO_count = sizeof(frame_vertices) / sizeof(frame_vertices[0]);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(frame_vertices), frame_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// particle quad mesh
	float quad_vertices[][4] = {
		{ -0.5f, -0.5f, 0.0f, 0.0f },
		{ 0.5f, -0.5f, 1.0f, 0.0f },
		{ -0.5f, 0.5f, 0.0f, 1.0f },
		{ -0.5f, 0.5f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, 1.0f, 0.0f },
		{ 0.5f, 0.5f, 1.0f, 1.0f },
	};
	glGenVertexArrays(1, &VAO);
	res->meshes.particleVAO = VAO;
	res->meshes.particleVAO_count = sizeof(quad_vertices) / sizeof(quad_vertices[0]);
	for (int i = 0; i < res->meshes.particleVAO_count; i++) {
		quad_vertices[i][2] /= BLOCK_TEXTURE_ROW_COUNT * 8;
		quad_vertices[i][3] /= BLOCK_TEXTURE_ROW_COUNT * 8;
	}
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// init some uniforms
	glUseProgram(res->shaders.ui);
	glUniform1i(res->shaders.ui_location.useTexture, 0);
	glUniform1i(res->shaders.ui_location.useTextureAlpha, 0);
	glUniform1i(res->shaders.ui_location.use_uv_offset, 0);
	glUseProgram(res->shaders.basic);
	vec3 light = {-2.0f, -1.0f, 0.5f};
	glm_vec3_normalize(light);
	glUniform3fv(res->shaders.basic_location.light, 1, (float*)light);
	glUniform1i(res->shaders.basic_location.use_uv_offset, 0);
	glUniform1i(res->shaders.basic_location.gamma_correct, 1);

	// init gl
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// init block icons renderer
	unsigned int fbo, rbo;
	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, BLOCK_ICON_SIZE, BLOCK_ICON_SIZE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	mat4 model, proj, view;
	mat3 normal;
	glm_mat4_identity(model);
	glm_ortho(-1, 1, 1, -1, 0.1f, 10.0f, proj); // will flip again in 2d render
	glm_lookat((vec3){2.0f, 1.5f, 2.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0, 1, 0}, view);
	glm_mat3_identity(normal);

	glUseProgram(res->shaders.basic);
	glUniformMatrix4fv(res->shaders.basic_location.model, 1, GL_FALSE, (float*)model);
	glUniformMatrix4fv(res->shaders.basic_location.proj, 1, GL_FALSE, (float*)proj);
	glUniformMatrix4fv(res->shaders.basic_location.view, 1, GL_FALSE, (float*)view);
	glUniformMatrix3fv(res->shaders.basic_location.normal_matrix, 1, GL_FALSE, (float*)normal);
	glUniform1i(res->shaders.basic_location.use_uv_offset, 1);
	glUniform1i(res->shaders.basic_location.gamma_correct, 0);
	vec3 item_light = {-0.1f, -2.0f, -0.8f};
	glm_vec3_normalize(item_light);
	glUniform3fv(res->shaders.basic_location.light, 1, (float*)item_light);
	glViewport(0, 0, BLOCK_ICON_SIZE, BLOCK_ICON_SIZE);
	glBindVertexArray(res->meshes.cubeVAO);
	for (int i = 0; i < ITEM_BLOCK_MAX; i++) {
		if (!item_is3d((ItemID)i)) {
			continue;
		}

		unsigned int tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BLOCK_ICON_SIZE, BLOCK_ICON_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
		// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, res->textures.blocks);
		const int *textures = block_get(block_ofItem((ItemID)i))->textures;
		for (int f = 0; f < 6; f++) {
			float uv[2];
			texture_blockUVoffset(textures[f], uv);
			glUniform2f(res->shaders.basic_location.uv_offset, uv[0], uv[1]);
			glDrawArrays(GL_TRIANGLES, f * res->meshes.cubeVAO_count / 6, res->meshes.cubeVAO_count / 6);
		}

		res->textures.cube_icons[i].texture = tex;
	}
	glUniform1i(res->shaders.basic_location.gamma_correct, 1);
	glUniform1i(res->shaders.basic_location.use_uv_offset, 0);
	glUniform3fv(res->shaders.basic_location.light, 1, (float*)light);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
