#include "camera.h"
#include "../util/mem.h"
#include "../util/props.h"
#include <math.h>

#include "../../third_party/cglm/include/cglm/cam.h"
#include "../../third_party/cglm/include/cglm/quat.h"
#include "../../third_party/cglm/include/cglm/vec3.h"
#include "../entity/player.h"
#include "../entity/bodies.h"

#define rdcam_SPEED_HALF_MAX 0.2f

Camera *newCamera(vec3 position, float aspect, CameraType type) {
	Camera *camera = zalloc(1, sizeof(Camera));
	glm_vec3_copy(position, camera->position);
	glm_quat_identity(camera->rotation);
	camera->fov = M_PI / 2;
	camera->aspect = aspect;
	camera->near = 0.1f;
	camera->far = 1000.0f;

	camera->type = type;
	if (type == CameraType_FPS) {
		camera->sensitivity = 0.01f;
		camera->speed = 0.05f;
	} else if (type == CameraType_RD) {
		// now this mean max sensitivity
		camera->sensitivity = 0.1f;
		camera->speed = 0;
	}

	return camera;
}

void camera_rotate(Camera *camera, vec3 axis, float rad) {
	versor rotation;
	glm_quatv(rotation, rad, axis);
	glm_quat_mul(rotation, camera->rotation, camera->rotation);
	glm_quat_normalize(camera->rotation);
}

void camera_move(Camera *camera, vec3 direction, float distance) {
	vec3 offset;
	glm_vec3_scale(direction, distance, offset);
	glm_vec3_add(camera->position, offset, camera->position);
}

void camera_attach(Camera *camera, Entity *player) {
	camera->player = player;
}

void camera_updatePos(Camera *camera) {
	FORR(3) {
		camera->prev_position[i] = camera->position[i];
	}
	camera->position[0] = camera->player->position.x;
	camera->position[1] = camera->player->position.y + PLAYER_EYE_OFFSET_Y;
	camera->position[2] = camera->player->position.z;
}
void camera_updateRot(Camera *camera) {
	FORR(4) {
		camera->rotation[i] = camera->player->rotation[i];
	}
}

void camera_updateMatrix(Camera *camera, float alpha) {
	vec3 front = {1.0f, 0.0f, 0.0f};
	vec3 up = {0.0f, 1.0f, 0.0f};
	glm_quat_rotatev(camera->rotation, front, front);
	glm_quat_rotatev(camera->rotation, up, up);

	vec3 position;
	glm_vec3_lerp(camera->prev_position, camera->position, alpha, position);

	// view
	vec3 target;
	glm_vec3_add(position, front, target);
	glm_lookat(position, target, up, camera->view);

	// proj
	glm_perspective(camera->fov, camera->aspect, camera->near, camera->far, camera->proj);
}

void fpscam_get(Camera *camera, vec3 front, vec3 up, vec3 right) {
	front[0] = 1;
	front[1] = 0;
	front[2] = 0;
	glm_quat_rotatev(camera->rotation, front, front);
	front[1] = 0;
	glm_vec3_normalize(front);
	up[0] = 0;
	up[1] = 1;
	up[2] = 0;
	glm_vec3_crossn(front, up, right);
}

void fpscam_forward(Camera *camera) {
	vec3 front, up, right;
	fpscam_get(camera, front, up, right);
	camera_move(camera, front, camera->speed);
}
void fpscam_backward(Camera *camera) {
	vec3 front, up, right;
	fpscam_get(camera, front, up, right);
	camera_move(camera, front, -camera->speed);
	// logd("front: %.2f %.2f %.2f\t\tup: %.2f %.2f %.2f\t\tright: %.2f %.2f %.2f",
	// 		front[0], front[1], front[2], up[0], up[1], up[2], right[0], right[1], right[2]);
}
void fpscam_left(Camera *camera) {
	vec3 front, up, right;
	fpscam_get(camera, front, up, right);
	camera_move(camera, right, -camera->speed);
}
void fpscam_right(Camera *camera) {
	vec3 front, up, right;
	fpscam_get(camera, front, up, right);
	camera_move(camera, right, camera->speed);
}
void fpscam_up(Camera *camera) {
	camera_move(camera, (vec3){0, 1, 0}, camera->speed);
}
void fpscam_down(Camera *camera) {
	camera_move(camera, (vec3){0, 1, 0}, -camera->speed);
}
void fpscam_rotate(Camera *camera, int dx, int dy) {
	vec3 front, up, right;
	fpscam_get(camera, front, up, right);
	camera_rotate(camera, right, -dy * camera->sensitivity);

	vec3 fr = {1, 0, 0};
	glm_quat_rotatev(camera->rotation, fr, fr);
	if (fabsf(glm_vec3_dot(fr, up)) > 0.99f) {
		// back
		camera_rotate(camera, right, dy * camera->sensitivity);
	}

	/* vertical first, then horizontal */
	camera_rotate(camera, up, -dx * camera->sensitivity);
}

void rdcam_get(Camera *camera, vec3 front, vec3 up, vec3 right) {
	front[0] = 1;
	front[1] = 0;
	front[2] = 0;
	up[0] = 0;
	up[1] = 1;
	up[2] = 0;
	glm_quat_rotatev(camera->rotation, front, front);
	glm_quat_rotatev(camera->rotation, up, up);
	glm_vec3_crossn(front, up, right);
}

void rdcam_throttle_high(Camera *camera, float ratio) {
	camera->speed = rdcam_SPEED_HALF_MAX + ratio * rdcam_SPEED_HALF_MAX;
}
void rdcam_throttle_low(Camera *camera, float ratio) {
	camera->speed = rdcam_SPEED_HALF_MAX - ratio * rdcam_SPEED_HALF_MAX;
}
void rdcam_yaw_left(Camera *camera, float ratio) {
	vec3 front, up, right;
	rdcam_get(camera, front, up, right);
	camera_rotate(camera, up, ratio * camera->sensitivity);
}
void rdcam_yaw_right(Camera *camera, float ratio) {
	vec3 front, up, right;
	rdcam_get(camera, front, up, right);
	camera_rotate(camera, up, -ratio * camera->sensitivity);
}
void rdcam_pitch_forward(Camera *camera, float ratio) {
	vec3 front, up, right;
	rdcam_get(camera, front, up, right);
	camera_rotate(camera, right, ratio * camera->sensitivity);
}
void rdcam_pitch_backward(Camera *camera, float ratio) {
	vec3 front, up, right;
	rdcam_get(camera, front, up, right);
	camera_rotate(camera, right, -ratio * camera->sensitivity);
}
void rdcam_roll_left(Camera *camera, float ratio) {
	vec3 front, up, right;
	rdcam_get(camera, front, up, right);
	camera_rotate(camera, front, -ratio * camera->sensitivity);
}
void rdcam_roll_right(Camera *camera, float ratio) {
	vec3 front, up, right;
	rdcam_get(camera, front, up, right);
	camera_rotate(camera, front, ratio * camera->sensitivity);
}
void rdcam_step(Camera *camera) {
	vec3 front, up, right;
	rdcam_get(camera, front, up, right);
	glm_vec3_scale(up, camera->speed, up);
	vec3 g = {0, -0.4 * rdcam_SPEED_HALF_MAX, 0};
	vec3 v;
	glm_vec3_add(up, g, v);
	camera_move(camera, v, 1.0f);
}
