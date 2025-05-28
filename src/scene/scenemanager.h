#ifndef _ICRAFT_render_ui_scenemanager_h
#define _ICRAFT_render_ui_scenemanager_h

#include <SDL2/SDL_events.h>
#include "scene.h"

// how manys scenes can be shown in one time
#define MAX_SCENES 10
// how many scenes we prepared
#define SCENES_LIB_VOLUME 32

typedef struct SceneManager {
	// current showing scenes, overlapped
	Scene *scenes[MAX_SCENES];
	int scene_count;

	// scenes we have
	Scene *library[SCENES_LIB_VOLUME];
	int library_count;
} SceneManager;

// register all scenes
void sceneManager_init();

// add scene to top
void sceneManager_push(const char *name);

// remove top scene
void sceneManager_pop();

// remove specified scene
void sceneManager_remove(const char *name);

// just pop and push
void sceneManager_switchTo(const char *sceneName);

// see top scene
const char *sceneManager_peekName();

// check whether scene is in stack
bool sceneManager_inStack(const char *name);

void sceneManager_handle(SDL_Event *event);
void sceneManager_update();
void sceneManager_render();

#endif
