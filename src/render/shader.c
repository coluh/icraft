#include "shader.h"
#include <stdio.h>
#include "../util/mem.h"

#include <glad/glad.h>

static char *read_file(const char *path) {
	FILE *fp = fopen(path, "r");

	if (fp == NULL) perror(path);

	fseek(fp, 0L, SEEK_END);
	int length = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	char *buffer = zalloc(length + 1, 1);
	int bytesRead = fread(buffer, 1, length, fp);
	if (bytesRead != length) perror("fread lack bytes");
	buffer[length] = 0;
	return buffer;
}

int shader_get(const char *vertexShader, const char *fragmentShader) {

	const char *vertexShaderCode = read_file(vertexShader);
	const char *fragmentShaderCode = read_file(fragmentShader);

	int success;
	char infoLog[512];
	int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShaderCode, NULL);
	free((void*)vertexShaderCode);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		fprintf(stderr, "vertex shader compile failed: %s\n", infoLog);
	}
	int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
	free((void*)fragmentShaderCode);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		fprintf(stderr, "fragment shader compile failed: %s\n", infoLog);
	}

	int program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		fprintf(stderr, "shader link failed: %s\n", infoLog);
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	return program;
}


