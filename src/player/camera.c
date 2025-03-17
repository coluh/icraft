#include "camera.h"
#include "../util/mem.h"
#include <math.h>

#include "../../third_party/cglm/include/cglm/cam.h"
#include "../../third_party/cglm/include/cglm/quat.h"
#include "../../third_party/cglm/include/cglm/vec3.h"

struct FPSCamera FPSCamera;
static void fps_forward(Camera *camera);
static void fps_backward(Camera *camera);
static void fps_left(Camera *camera);
static void fps_right(Camera *camera);
static void fps_up(Camera *camera);
static void fps_down(Camera *camera);
static void fps_rotate(Camera *camera, int dx, int dy);

struct RDCamera RDCamera;
static void rd_throttle_high(Camera *camera, float ratio);
static void rd_throttle_low(Camera *camera, float ratio);
static void rd_yaw_left(Camera *camera, float ratio);
static void rd_yaw_right(Camera *camera, float ratio);
static void rd_pitch_forward(Camera *camera, float ratio);
static void rd_pitch_backward(Camera *camera, float ratio);
static void rd_roll_left(Camera *camera, float ratio);
static void rd_roll_right(Camera *camera, float ratio);
static void rd_step(Camera *camera);

#define RD_SPEED_HALF_MAX 0.2f

void camera_init() {
	FPSCamera.forward = fps_forward;
	FPSCamera.backward = fps_backward;
	FPSCamera.left = fps_left;
	FPSCamera.right = fps_right;
	FPSCamera.up = fps_up;
	FPSCamera.down = fps_down;
	FPSCamera.rotate = fps_rotate;
	RDCamera.throttle_high = rd_throttle_high;
	RDCamera.throttle_low = rd_throttle_low;
	RDCamera.yaw_left = rd_yaw_left;
	RDCamera.yaw_right = rd_yaw_right;
	RDCamera.pitch_forward = rd_pitch_forward;
	RDCamera.pitch_backward = rd_pitch_backward;
	RDCamera.roll_left = rd_roll_left;
	RDCamera.roll_right = rd_roll_right;
	RDCamera.step = rd_step;
}

Camera *newCamera(vec3 position, float aspect, CameraType type) {
	Camera *camera = zalloc(1, sizeof(Camera));
	glm_vec3_copy(position, camera->position);
	glm_quat_identity(camera->rotation);
	camera->fov = M_PI / 2;
	camera->aspect = aspect;
	camera->near = 0.1f;
	camera->far = 100.0f;

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

void camera_updateMatrix(Camera *camera) {
	vec3 front = {1.0f, 0.0f, 0.0f};
	vec3 up = {0.0f, 1.0f, 0.0f};
	glm_quat_rotatev(camera->rotation, front, front);
	glm_quat_rotatev(camera->rotation, up, up);

	// view
	vec3 target;
	glm_vec3_add(camera->position, front, target);
	glm_lookat(camera->position, target, up, camera->view);

	// proj
	glm_perspective(camera->fov, camera->aspect, camera->near, camera->far, camera->proj);
}

static void fps_get(Camera *camera, vec3 front, vec3 up, vec3 right) {
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

static void fps_forward(Camera *camera) {
	vec3 front, up, right;
	fps_get(camera, front, up, right);
	camera_move(camera, front, camera->speed);
}
static void fps_backward(Camera *camera) {
	vec3 front, up, right;
	fps_get(camera, front, up, right);
	camera_move(camera, front, -camera->speed);
	// logd("front: %.2f %.2f %.2f\t\tup: %.2f %.2f %.2f\t\tright: %.2f %.2f %.2f",
	// 		front[0], front[1], front[2], up[0], up[1], up[2], right[0], right[1], right[2]);
}
static void fps_left(Camera *camera) {
	vec3 front, up, right;
	fps_get(camera, front, up, right);
	camera_move(camera, right, -camera->speed);
}
static void fps_right(Camera *camera) {
	vec3 front, up, right;
	fps_get(camera, front, up, right);
	camera_move(camera, right, camera->speed);
}
static void fps_up(Camera *camera) {
	camera_move(camera, (vec3){0, 1, 0}, camera->speed);
}
static void fps_down(Camera *camera) {
	camera_move(camera, (vec3){0, 1, 0}, -camera->speed);
}
static void fps_rotate(Camera *camera, int dx, int dy) {
	vec3 front, up, right;
	fps_get(camera, front, up, right);
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

static void rd_get(Camera *camera, vec3 front, vec3 up, vec3 right) {
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

static void rd_throttle_high(Camera *camera, float ratio) {
	camera->speed = RD_SPEED_HALF_MAX + ratio * RD_SPEED_HALF_MAX;
}
static void rd_throttle_low(Camera *camera, float ratio) {
	camera->speed = RD_SPEED_HALF_MAX - ratio * RD_SPEED_HALF_MAX;
}
static void rd_yaw_left(Camera *camera, float ratio) {
	vec3 front, up, right;
	rd_get(camera, front, up, right);
	camera_rotate(camera, up, ratio * camera->sensitivity);
}
static void rd_yaw_right(Camera *camera, float ratio) {
	vec3 front, up, right;
	rd_get(camera, front, up, right);
	camera_rotate(camera, up, -ratio * camera->sensitivity);
}
static void rd_pitch_forward(Camera *camera, float ratio) {
	vec3 front, up, right;
	rd_get(camera, front, up, right);
	camera_rotate(camera, right, ratio * camera->sensitivity);
}
static void rd_pitch_backward(Camera *camera, float ratio) {
	vec3 front, up, right;
	rd_get(camera, front, up, right);
	camera_rotate(camera, right, -ratio * camera->sensitivity);
}
static void rd_roll_left(Camera *camera, float ratio) {
	vec3 front, up, right;
	rd_get(camera, front, up, right);
	camera_rotate(camera, front, -ratio * camera->sensitivity);
}
static void rd_roll_right(Camera *camera, float ratio) {
	vec3 front, up, right;
	rd_get(camera, front, up, right);
	camera_rotate(camera, front, ratio * camera->sensitivity);
}
static void rd_step(Camera *camera) {
	vec3 front, up, right;
	rd_get(camera, front, up, right);
	glm_vec3_scale(up, camera->speed, up);
	vec3 g = {0, -0.4 * RD_SPEED_HALF_MAX, 0};
	vec3 v;
	glm_vec3_add(up, g, v);
	camera_move(camera, v, 1.0f);
}
