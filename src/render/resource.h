#ifndef _ICRAFT_render_resource_h
#define _ICRAFT_render_resource_h

typedef struct ResourceManager {
	struct ShaderManager {
		int basic;
		struct {
			int model;
			int view;
			int proj;
		} basic_location;
		int ui;
		struct {
			int model;
			int proj;
			int color;
			int useTexture;
		} ui_location;
	} shaders;
	struct TextureManager {
		unsigned int blocks;
	} textures;
	struct MeshManager {
		unsigned int rectangleVAO;
		unsigned int rectangleVAO_count;
		unsigned int cubeVAO;
		unsigned int cubeVAO_count;
	} meshes;
} ResourceManager;

void resource_init();

#endif
