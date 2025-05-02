#include "scenemanager.h"
#include <string.h>
#include "scene.h"
#include "../util/props.h"
#include "../util/log.h"
#include "ui/gui.h"
#include "di/di.h"

// how manys scenes can be shown in one time
#define MAX_SCENES 10
// how many scenes we prepared
#define SCENES_LIB_VOLUME 32

static struct {
	// current showing scenes, overlapped
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
	registerScene(di_ofMain());
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

Scene *sceneManager_peek() {
	if (sceneManager.scene_count == 0) {
		return NULL;
	}
	return sceneManager.scenes[sceneManager.scene_count-1];
}

void sceneManager_push(const char *name) {
	if (sceneManager.scene_count == MAX_SCENES) {
		logw("Too many scenes");
		return;
	}

	sceneManager.scenes[sceneManager.scene_count] = getScene(name);
	sceneManager.scene_count++;
	Scene *current = sceneManager_peek();
	if (current->on_enter != NULL) {
		current->on_enter(current);
	}
}

void sceneManager_pop() {
	if (sceneManager.scene_count == 0) {
		logw("scene stack empty");
		return;
	}

	Scene *current = sceneManager_peek();
	if (current != NULL && current->on_exit != NULL) {
		current->on_exit(current);
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

const char *sceneManager_peekName() {
	Scene *s = sceneManager.scenes[sceneManager.scene_count-1];
	if (s == NULL) {
		return NULL;
	} else {
		return scene_getName(s);
	}
}



