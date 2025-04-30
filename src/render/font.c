#include "font.h"
#include "../util/log.h"
#include "../util/props.h"
#include "render_2d.h"
#include <glad/glad.h>

#include <ft2build.h>
#include <stdint.h>
#include FT_FREETYPE_H

typedef struct Character {
	GLuint textureID;
	int width;
	int height;
	int bearingX;
	int bearingY;
	int advance;
} Character;

FT_Library ft;
FT_Face face;
Character characters[65536];

void font_init(const char *font_path) {

	if (FT_Init_FreeType(&ft)) {
		loge("Fail to init FreeType");
		return;
	}

	if (FT_New_Face(ft, font_path, 0, &face)) {
		loge("Fail to load font: %s", font_path);
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, 24);

	return;
}

static void font_loadCharacter(unsigned int ch) {

	if (FT_Load_Char(face, ch, FT_LOAD_RENDER)) {
		logw("Fail to load glyph for %u", ch);
		return;
	}

	FT_GlyphSlot g = face->glyph;

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	if (glGetError() != GL_NO_ERROR)
		logw("Texture fail to generate");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
	if (glGetError() != GL_NO_ERROR)
		logw("Texture fail to upload");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	characters[ch].textureID = tex;
	characters[ch].width = g->bitmap.width;
	characters[ch].height = g->bitmap.rows;
	characters[ch].bearingX = g->bitmap_left;
	characters[ch].bearingY = g->bitmap_top;
	characters[ch].advance = g->advance.x >> 6;
}

static const char *utf8next(const char *s, uint32_t *out) {
	unsigned char c = *s;
	if (c < 0b10000000) {
		*out = c;
		return s + 1;
	} else if ((c >> 5) == 0b110) {
		*out = ((c & 0b11111) << 6) | (s[1] & 0b111111);
		return s + 2;
	} else if ((c >> 4) == 0b1110) {
		*out = ((c & 0b1111) << 12) | ((s[1] & 0b111111) << 6) | (s[2] & 0b111111);
		return s + 3;
	} else if ((c >> 3) == 0b11110) {
		*out = ((c & 0b111) << 18) | ((s[1] & 0b111111) << 12) | ((s[2] & 0b111111) << 6) | (s[3] & 0b111111);
		return s + 4;
	} else {
		*out = '?';
		return s + 1;
	}
}

void font_queryText(const char *utf8str, float scale, int *width, int *height) {

	*width = 0;
	*height = 0;

	uint32_t ch;
	int b;
	int w;
	int a;
	int ymin = 999999;
	int ymax = -1;
	int first = 1;
	while (*utf8str) {
		utf8str = utf8next(utf8str, &ch);
		if (characters[ch].textureID == 0) {
			// logv("Load %c", ch);
			font_loadCharacter(ch);
		}

		Character *c = &characters[ch];
		if (first) {
			*width -= c->bearingX * scale;
			first = 0;
		}
		ymin = MIN(ymin, -c->bearingY * scale);
		ymax = MAX(ymax, (c->height - c->bearingY) * scale);
		b = c->bearingX * scale;
		w = c->width * scale;
		a = c->advance * scale;

		*width += c->advance * scale;
	}
	*width += -a + b + w;
	*height = ymax - ymin;
}

void font_drawTextFromOrigin(const char *utf8str, float x, float y, float scale) {
	uint32_t ch;
	while (*utf8str) {
		utf8str = utf8next(utf8str, &ch);
		if (characters[ch].textureID == 0) {
			// logv("Load %c", ch);
			font_loadCharacter(ch);
		}

		Character *c = &characters[ch];
		int xpos = x + c->bearingX * scale;
		int ypos = y - c->bearingY * scale;
		int w = c->width * scale;
		int h = c->height * scale;

		// glBindTexture(GL_TEXTURE_2D, c->textureID);
		twod_drawTexture(xpos, ypos, w, h, c->textureID);

		x += c->advance * scale;
	}
}

void font_drawText(const char *utf8str, float x, float y, float scale) {
	uint32_t ch;
	int first = 1;
	while (*utf8str) {
		utf8str = utf8next(utf8str, &ch);
		if (characters[ch].textureID == 0) {
			// logv("Load %c", ch);
			font_loadCharacter(ch);
		}

		Character *c = &characters[ch];
		if (first) {
			x -= c->bearingX * scale;
			y += c->bearingY * scale;
			first = 0;
		}
		int xpos = x + c->bearingX * scale;
		int ypos = y - c->bearingY * scale;
		int w = c->width * scale;
		int h = c->height * scale;

		// glBindTexture(GL_TEXTURE_2D, c->textureID);
		twod_drawTexture(xpos, ypos, w, h, c->textureID);

		x += c->advance * scale;
	}
}
