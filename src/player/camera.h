#ifndef _ICRAFT_player_camera_h
#define _ICRAFT_player_camera_h

#include "../../third_party/cglm/include/cglm/types.h"
#include "player.h"

typedef enum CameraType {
	CameraType_FPS, CameraType_RD,
} CameraType;

typedef struct {
	vec3 position;
	versor rotation; // based on front=(1,0,0), up=(0,1,0)
			 // in fact all rotations are based on these
	float fov;       // vertical FOV (fovy)
	float aspect;    // width/height
	float near;
	float far;

	float sensitivity;
	float speed;
	CameraType type;

	Player *player;

	// used in computation
	mat4 view;
	mat4 proj;
} Camera;

Camera *newCamera(vec3 position, float aspect, CameraType type);
void camera_rotate(Camera *camera, vec3 axis, float rad);
void camera_move(Camera *camera, vec3 direction, float distance);
void camera_attach(Camera *camera, Player *player);
// when attached to something, you should update to get pos and rot
void camera_update(Camera *camera);

// re-compute view matrix, proj matrix
void camera_updateMatrix(Camera *camera);

/* FPS Camera */
void fpscam_forward(Camera *camera);
void fpscam_backward(Camera *camera);
void fpscam_left(Camera *camera);
void fpscam_right(Camera *camera);
void fpscam_up(Camera *camera);
void fpscam_down(Camera *camera);
void fpscam_rotate(Camera *camera, int dx, int dy);

/* Racing Drone */
// ratio is between 0.0f ~ 1.0f
void rdcam_throttle_high(Camera *camera, float ratio);
void rdcam_throttle_low(Camera *camera, float ratio);
void rdcam_yaw_left(Camera *camera, float ratio);
void rdcam_yaw_right(Camera *camera, float ratio);
void rdcam_pitch_forward(Camera *camera, float ratio);
void rdcam_pitch_backward(Camera *camera, float ratio);
void rdcam_roll_left(Camera *camera, float ratio);
void rdcam_roll_right(Camera *camera, float ratio);
// call me each tick
void rdcam_step(Camera *camera);


#endif
