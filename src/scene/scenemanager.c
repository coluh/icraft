#include "scenemanager.h"
#include <string.h>
#include "scene.h"
#include "../../util/props.h"
#include "../../util/log.h"
#include "gui/gui.h"

// how manys scenes can be shown in one time
#define MAX_SCENES 10
// how many scenes we prepared
#define SCENES_LIB_VOLUME 32

static struct {
	// current showing scenes
	Scene *scenes[MAX_SCENES];
	int scene_count;

	// scenes we have
	Scene *library[SCENES_LIB_VOLUME];
	int library_count;
} sceneManager;

static void registerScene(Scene *scene) {
	sceneManager.library[sceneManager.library_count] = scene;
	sceneManager.library_count++;
}

void sceneManager_init() {
	registerScene(gui_ofInGame());
	registerScene(gui_ofEscape());
}

static Scene *getScene(const char *name) {
	if (name == NULL) {
		loge("Scene name required");
	}
	FORR(sceneManager.library_count) {
		Scene *s = sceneManager.library[i];
		if (strcmp(scene_getName(s), name) == EQUAL) {
			return s;
		}
	}
	loge("No such scene: %s", name);
	return NULL;
}

void sceneManager_push(const char *name) {
	if (sceneManager.scene_count == MAX_SCENES) {
		logw("Too many scenes");
		return;
	}
	sceneManager.scenes[sceneManager.scene_count] = getScene(name);
	sceneManager.scene_count++;
}

void sceneManager_pop() {
	if (sceneManager.scene_count == 0) {
		logw("scene stack empty");
		return;
	}
	sceneManager.scene_count--;
}

void sceneManager_switchTo(const char *name) {
	sceneManager_pop();
	sceneManager_push(name);
}

void sceneManager_handle(SDL_Event *event) {
	FORR(sceneManager.scene_count) {
		scene_handle(sceneManager.scenes[i], event);
	}
}

void sceneManager_update() {
	FORR(sceneManager.scene_count) {
		scene_update(sceneManager.scenes[i]);
	}
}

void sceneManager_render() {
	FORR(sceneManager.scene_count) {
		scene_render(sceneManager.scenes[i]);
	}
}

const char *sceneManager_peek() {
	return scene_getName(sceneManager.scenes[sceneManager.scene_count]);
}



