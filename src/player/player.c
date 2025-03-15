#include "player.h"
#include "camera.h"
#include "../util/mem.h"
#include "../render/window.h"

struct Player {
	Camera *camera;
};

Player *newPlayer(float x, float y, float z) {
	Player *p = zalloc(1, sizeof(Player));
	p->camera = newCamera((vec3){x, y, z}, (float)window_getWidth()/window_getHeight(), CameraType_FPS);
	return p;
}

Camera *player_getCamera(Player *p) { return p->camera; }
