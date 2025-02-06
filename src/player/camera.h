#ifndef _ICRAFT_player_camera_h
#define _ICRAFT_player_camera_h

#include "../../third_party/cglm/include/cglm/types.h"

typedef enum CameraType {
	CameraType_FPS, CameraType_RD,
} CameraType;

typedef struct {
	vec3 position;
	versor rotation; // based on front=(1,0,0), up=(0,1,0)
	float fov;       // vertical FOV (fovy)
	float aspect;    // width/height
	float near;
	float far;

	float sensitivity;
	float speed;
	CameraType type;
	vec3 v; // for RD Camera

	// used in computation
	mat4 view;
	mat4 proj;
} Camera;

void camera_init();

Camera *newCamera(vec3 position, float aspect, CameraType type);
void camera_rotate(Camera *camera, vec3 axis, float rad);
void camera_move(Camera *camera, vec3 direction, float distance);

// update view matrix, proj matrix
void camera_update(Camera *camera);

/* FPS Camera */
extern struct FPSCamera {
	void (*forward)(Camera *camera);
	void (*backward)(Camera *camera);
	void (*left)(Camera *camera);
	void (*right)(Camera *camera);
	void (*up)(Camera *camera);
	void (*down)(Camera *camera);
	void (*rotate)(Camera *camera, int dx, int dy);
} FPSCamera;

/* Racing Drone */
extern struct RDCamera {
	// ratio is between 0.0f ~ 1.0f
	void (*throttle_high)(Camera *camera, float ratio);
	void (*throttle_low)(Camera *camera, float ratio);
	void (*yaw_left)(Camera *camera, float ratio);
	void (*yaw_right)(Camera *camera, float ratio);
	void (*pitch_forward)(Camera *camera, float ratio);
	void (*pitch_backward)(Camera *camera, float ratio);
	void (*roll_left)(Camera *camera, float ratio);
	void (*roll_right)(Camera *camera, float ratio);
	// call me each tick
	void (*step)(Camera *camera);
} RDCamera;

#endif
