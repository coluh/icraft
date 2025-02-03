#ifndef _ICRAFT_player_camera_h
#define _ICRAFT_player_camera_h

#include "../../third_party/cglm/include/cglm/types.h"

typedef struct {
	vec3 position;
	versor rotation; // based on front=(1,0,0), up=(0,1,0)
	float fov;       // vertical FOV (fovy)
	float aspect;    // width/height
	float near;
	float far;

	float sensitivity;
	float speed;

	// used in computation
	mat4 view;
	mat4 proj;
} Camera;

void camera_init();

Camera *newCamera(vec3 position, float aspect);
void camera_rotate(Camera *camera, vec3 axis, float rad);
void camera_move(Camera *camera, vec3 direction, float distance);

extern struct FPSCamera {
	void (*forward)(Camera *camera);
	void (*backward)(Camera *camera);
	void (*left)(Camera *camera);
	void (*right)(Camera *camera);
	void (*up)(Camera *camera);
	void (*down)(Camera *camera);
	void (*rotate)(Camera *camera, int dx, int dy);
} FPSCamera;

// update view matrix, proj matrix
void camera_update(Camera *camera);

#endif
