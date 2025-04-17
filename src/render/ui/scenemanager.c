#include "scenemanager.h"
#include "scene.h"
#include "../../util/log.h"

#define SCENEMANAGER_MAX_SCENESTACK_DEPTH 32

static struct {
	Scene *list[SCENEMANAGER_MAX_SCENESTACK_DEPTH];
	int count;
} sceneStack;

void sceneManager_init() {
	sceneStack.count = 0;
}

void sceneManager_handle(SDL_Event *event) {
	if (sceneStack.count == 0) return;
	scene_update(sceneStack.list[sceneStack.count-1], event);
}

void sceneManager_render() {
	if (sceneStack.count == 0) return;
	scene_render(sceneStack.list[sceneStack.count-1]);
}

void sceneManager_push(Scene *scene) {
	if (sceneStack.count >= SCENEMANAGER_MAX_SCENESTACK_DEPTH) {
		loge("StackOverflow");
	}

	sceneStack.list[sceneStack.count] = scene;
	sceneStack.count++;
}

Scene *sceneManager_pop() {
	if (sceneStack.count == 0) return NULL;
	Scene *r = sceneStack.list[sceneStack.count-1];
	sceneStack.count--;
	return r;
}

const char *sceneManager_peekName() {
	if (sceneStack.count == 0) return NULL;
	return scene_getName(sceneStack.list[sceneStack.count-1]);
}

