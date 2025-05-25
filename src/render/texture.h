#ifndef _ICRAFT_render_texture_h
#define _ICRAFT_render_texture_h

#define BLOCK_TEXTURE_SIZE 16
// how many textures in one row. Also column count
#define BLOCK_TEXTURE_ROW_COUNT 16

unsigned int texture_load(const char *texture_path);
void texture_blockUVoffset(int index, float uv[]);

#endif
