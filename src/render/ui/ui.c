#include "ui.h"
#include "scene.h"
#include "../../util/log.h"

#define UI_MAX_SCENESTACK_DEPTH 32

static struct {
	Scene *list[UI_MAX_SCENESTACK_DEPTH];
	int count;
} sceneStack;

void ui_init() {
	sceneStack.count = 0;
}

void ui_handle(SDL_Event *event) {
	if (sceneStack.count == 0) return;
	scene_update(sceneStack.list[sceneStack.count-1], event);
}

void ui_render() {
	if (sceneStack.count == 0) return;
	scene_render(sceneStack.list[sceneStack.count-1]);
}

void ui_pushScene(Scene *scene) {
	if (sceneStack.count >= UI_MAX_SCENESTACK_DEPTH) {
		loge("StackOverflow");
	}

	sceneStack.list[sceneStack.count] = scene;
	sceneStack.count++;
}

Scene *ui_popScene() {
	if (sceneStack.count == 0) return NULL;
	Scene *r = sceneStack.list[sceneStack.count-1];
	sceneStack.count--;
	return r;
}
