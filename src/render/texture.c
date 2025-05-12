#include "texture.h"
#include <glad/glad.h>
#include <stdbool.h>
#include <stdint.h>
#include "../util/log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../third_party/stb/stb_image.h"

unsigned int texture_load(const char *texture_path) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, n_channel;
	stbi_set_flip_vertically_on_load(true);
	uint8_t *data = stbi_load(texture_path, &width, &height, &n_channel, 0);
	Assert(data != NULL, "fail to load texture: %s", texture_path);

	Assert(n_channel == 4, "missing alpha channel");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	return texture;
}
