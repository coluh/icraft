#include "scenemanager.h"
#include <stdbool.h>
#include <string.h>
#include "cui/cui.h"
#include "scene.h"
#include "../util/props.h"
#include "../util/mem.h"
#include "../util/log.h"
#include "gui/gui.h"
#include "di/di.h"
#include "hud/hud.h"
#include "../game.h"
#include "../render/window.h"

extern Game g;

static void registerScene(Scene *scene) {
	g.scene_manager->library[g.scene_manager->library_count] = scene;
	g.scene_manager->library_count++;
}

void sceneManager_init() {
	g.scene_manager = zalloc(1, sizeof(SceneManager));
	registerScene(hud_ofMain());
	registerScene(gui_ofEscape());
	registerScene(di_ofMain());
	registerScene(cui_ofBackpack());
	window_setSize(0, 0);
}

static Scene *getScene(const char *name) {
	if (name == NULL) {
		loge("Scene name required");
	}
	FORR(g.scene_manager->library_count) {
		Scene *s = g.scene_manager->library[i];
		if (strcmp(scene_getName(s), name) == EQUAL) {
			return s;
		}
	}
	loge("No such scene: %s", name);
	return NULL;
}

Scene *sceneManager_peek() {
	if (g.scene_manager->scene_count == 0) {
		return NULL;
	}
	return g.scene_manager->scenes[g.scene_manager->scene_count-1];
}

void sceneManager_push(const char *name) {
	if (g.scene_manager->scene_count == MAX_SCENES) {
		logw("Too many scenes");
		return;
	}

	g.scene_manager->scenes[g.scene_manager->scene_count] = getScene(name);
	g.scene_manager->scene_count++;
	Scene *current = sceneManager_peek();
	if (current->on_enter != NULL) {
		current->on_enter(current);
	}
	if (current->type == Scene_GUI) {
		setDefaultLayout(current);
	}
}

void sceneManager_pop() {
	if (g.scene_manager->scene_count == 0) {
		logw("scene stack empty");
		return;
	}

	Scene *current = sceneManager_peek();
	if (current != NULL && current->on_exit != NULL) {
		current->on_exit(current);
	}
	g.scene_manager->scene_count--;
}

void sceneManager_remove(const char *name) {
	FORR(g.scene_manager->scene_count) {
		Scene *s = g.scene_manager->scenes[i];
		if (strcmp(name, s->name) == EQUAL) {
			if (s->on_exit != NULL) {
				s->on_exit(s);
			}
			for (int j = i; j < g.scene_manager->scene_count-1; j++) {
				g.scene_manager[j] = g.scene_manager[j+1];
			}
			g.scene_manager->scene_count--;
			return;
		}
	}
	logw("remove a non-exist scene");
}

void sceneManager_switchTo(const char *name) {
	sceneManager_pop();
	sceneManager_push(name);
}

void sceneManager_handle(SDL_Event *event) {
	for (int i = g.scene_manager->scene_count-1; i >= 0; i--) {
		Scene *s = g.scene_manager->scenes[i];
		scene_handle(s, event);
		if (s->block_event) {
			return;
		}
	}
}

void sceneManager_update() {
	bool input = true;

	for (int i = g.scene_manager->scene_count-1; i >= 0; i--) {
		Scene *s = g.scene_manager->scenes[i];
		scene_update(s, input);
		if (s->block_event) {
			input = false;
		}
	}
}

void sceneManager_render() {
	FORR(g.scene_manager->scene_count) {
		scene_render(g.scene_manager->scenes[i]);
	}
}

const char *sceneManager_peekName() {
	Scene *s = g.scene_manager->scenes[g.scene_manager->scene_count-1];
	if (s == NULL) {
		return NULL;
	} else {
		return scene_getName(s);
	}
}

bool sceneManager_inStack(const char *name) {
	FORR(g.scene_manager->scene_count) {
		if (strcmp(name, g.scene_manager->scenes[i]->name) == EQUAL) {
			return true;
		}
	}
	return false;
}

