#ifndef _ICRAFT_render_resource_h
#define _ICRAFT_render_resource_h

// how to determine this value
#define ITEM_BLOCK_MAX 256

typedef struct ResourceManager {
	struct ShaderManager {
		int basic;
		struct {
			int model;
			int view;
			int proj;
			int normal_matrix;
			int light;
			int use_uv_offset;
			int uv_offset;
			int gamma_correct;
		} basic_location;
		int ui;
		struct {
			int model;
			int proj;
			int color;
			int useTexture;
			int useTextureAlpha;
			int use_uv_offset;
			int uv_offset;
		} ui_location;
	} shaders;
	struct TextureManager {
		unsigned int blocks;
		struct {
			// unsigned int fbo, rbo;
			// mat4 proj, view;
			unsigned int texture;
		} cube_icons[ITEM_BLOCK_MAX];
	} textures;
	struct MeshManager {
		unsigned int rectangleVAO;
		unsigned int rectangleVAO_count;
		unsigned int cubeVAO;
		unsigned int cubeVAO_count;
		unsigned int frameVAO;
		unsigned int frameVAO_count;
	} meshes;
} ResourceManager;

void resource_init();

#endif
