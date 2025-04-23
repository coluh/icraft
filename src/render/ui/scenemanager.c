#include "scenemanager.h"
#include <string.h>
#include "scene.h"
#include "../../util/props.h"
#include "../../util/log.h"
#include "gui/gui.h"

#define MAX_SCENE_COUNT 32

static struct {
	Scene *current;
	Scene *scenes[MAX_SCENE_COUNT];
	int scenes_n;
} sceneManager;

static void sceneManager_register(Scene *scene) {
	sceneManager.scenes[sceneManager.scenes_n] = scene;
	sceneManager.scenes_n++;
}

void sceneManager_init() {
	sceneManager_register(gui_ofInGame());
	sceneManager_register(gui_ofEscape());
}

void sceneManager_switchTo(const char *sceneName) {
	if (!sceneName) {
		sceneManager.current = NULL;
		return;
	}
	FORR(sceneManager.scenes_n) {
		Scene *s = sceneManager.scenes[i];
		if (strcmp(scene_getName(s), sceneName) == EQUAL) {
			sceneManager.current = s;
			scene_updateLayout(s);
			return;
		}
	}
	logd("Unknown GUI: %s", sceneName);
}

void sceneManager_handle(SDL_Event *event) {
	if (sceneManager.current) {
		scene_update(sceneManager.current, event);
	}
}

void sceneManager_render() {
	if (sceneManager.current) {
		scene_render(sceneManager.current);
	}
}

const char *sceneManager_getCurrent() {
	if (!sceneManager.current) return NULL;
	return scene_getName(sceneManager.current);
}



