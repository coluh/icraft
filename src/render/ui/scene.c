#include "scene.h"
#include "element.h"
#include "../../util/mem.h"
#include "../../render/window.h"

typedef struct Scene {
	const char *name;
	int x, y, w, h;
	uiElement *root;
} Scene;

Scene *newScene(const char *name, uiElement *root) {
	Scene *s = zalloc(1, sizeof(Scene));
	s->name = name;
	s->root = root;
	s->x = 0;
	s->y = 0;
	s->w = window_getWidth();
	s->h = window_getHeight();
	root->rect.x = s->w/6;
	root->rect.y = s->h/6;
	root->rect.w = s->w*2/3;
	root->rect.h = s->h*2/3;
	ui_arrangeLinearLayout(root);
	return s;
}

const char *scene_getName(Scene *scene) { return scene->name; }

void scene_free(Scene *scene) {}

void scene_update(Scene *scene, SDL_Event *event) {
	ui_updateElement(scene->root, event);
}

void scene_render(Scene *scene) {
	ui_renderElement(scene->root);
}
