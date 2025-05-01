#include "player.h"
#include "../util/mem.h"
#include "../util/props.h"
#include "../world/world.h"

#include "../../third_party/cglm/include/cglm/vec3.h"
#include "../../third_party/cglm/include/cglm/quat.h"

#define PLAYER_ROTATE_SENSI	0.003f
#define PLAYER_MOVE_SPEED	0.2f
#define PLAYER_JUMP_SPEED	4.2f

#define PLAYER_WIDTH	0.6
#define PLAYER_HEIGHT	1.8

#define GAME_UPDATE_DT 0.05f

struct Player {
	struct {
		float x, y, z;
	} pos;
	struct {
		float x, y, z;
	} v;
	bool on_ground;
	versor brot; // body rotation
	versor hrot; // head rotation
};

// this is for ?
static Player *currentPlayer;

void setPlayer(Player *p) {
	currentPlayer = p;
}

// get current player
Player *getPlayer() {
	return currentPlayer;
}

Player *newPlayer() {
	Player *p = zalloc(1, sizeof(Player));
	glm_quat_identity(p->hrot);
	glm_quat_identity(p->brot);

	player_setPos(p, -10, 24, -5);
	player_rotateHead(p, (float[]){0, 1, 0}, -0.1);
	return p;
}

float *player_getPos(Player *p) {
	return (float*)&p->pos;
}

void player_setPos(Player *p, float x, float y, float z) {
	p->pos.x = x;
	p->pos.y = y;
	p->pos.z = z;
}

void player_update(Player *p, World *w) {

	if (p->v.x != 0) {
		float nx = p->pos.x + p->v.x * GAME_UPDATE_DT;
		if (world_collide(w, BODY(nx-PLAYER_WIDTH/2, p->pos.y-PLAYER_HEIGHT/2, p->pos.z-PLAYER_WIDTH/2,
					PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH))) {
			p->v.x = 0;
		} else {
			p->pos.x = nx;
		}
	}

	if (p->v.y != 0) {
		float ny = p->pos.y + p->v.y * GAME_UPDATE_DT;
		if (world_collide(w, BODY(p->pos.x-PLAYER_WIDTH/2, ny-PLAYER_HEIGHT/2, p->pos.z-PLAYER_WIDTH/2,
					PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH))) {
			p->v.y = 0;
			p->on_ground = true;
		} else {
			p->pos.y = ny;
		}
	}

	if (p->v.z != 0) {
		float nz = p->pos.z + p->v.z * GAME_UPDATE_DT;
		if (world_collide(w, BODY(p->pos.x-PLAYER_WIDTH/2, p->pos.y-PLAYER_HEIGHT/2, nz-PLAYER_WIDTH/2,
					PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH))) {
			p->v.z = 0;
		} else {
			p->pos.z = nz;
		}
	}

	if (!p->on_ground) {
		p->v.y += -9.8 * GAME_UPDATE_DT;
	}
}

void player_copyTo(Player *p, vec3 pos, versor rot) {
	pos[0] = p->pos.x;
	pos[1] = p->pos.y;
	pos[2] = p->pos.z;
	FORR(4) rot[i] = p->hrot[i];
}

void player_move(Player *p, vec3 dir, float d) {
	vec3 off;
	glm_vec3_scale(dir, d, off);
	float *v = (float*)&p->pos;
	glm_vec3_add(v, off, v);
}

void player_rotateHead(Player *p, vec3 axis, float rad) {
	versor rotation;
	glm_quatv(rotation, rad, axis);
	glm_quat_mul(rotation, p->hrot, p->hrot);
	glm_quat_normalize(p->hrot); // is this necessary?
}

static void player_rotateBody(Player *p, vec3 axis, float rad) {
	versor rotation;
	glm_quatv(rotation, rad, axis);
	glm_quat_mul(rotation, p->brot, p->brot);
	glm_quat_normalize(p->brot); // is this necessary?
}


static void getv_upright(Player *p, vec3 front, vec3 up, vec3 right) {

	// get forward vector
	front[0] = 1;
	front[1] = 0;
	front[2] = 0;
	glm_quat_rotatev(p->hrot, front, front);

	// get upright front
	front[1] = 0;
	glm_vec3_normalize(front);

	// get upright up
	up[0] = 0;
	up[1] = 1;
	up[2] = 0;

	// get upright right
	glm_vec3_crossn(front, up, right);
}

void player_forward(void *p, SDL_Event *ev) {
	vec3 front, up, right;
	getv_upright(p, front, up, right);
	// if "walk" upright,0,0 ... if "swim" head forward ...
	player_move(p, front, PLAYER_MOVE_SPEED);
}
void player_backward(void *p, SDL_Event *ev) {
	vec3 front, up, right;
	getv_upright(p, front, up, right);
	player_move(p, front, -PLAYER_MOVE_SPEED);
}
void player_left(void *p, SDL_Event *ev) {
	vec3 front, up, right;
	getv_upright(p, front, up, right);
	player_move(p, right, -PLAYER_MOVE_SPEED);
}
void player_right(void *p, SDL_Event *ev) {
	vec3 front, up, right;
	getv_upright(p, front, up, right);
	player_move(p, right, PLAYER_MOVE_SPEED);
}
void player_up(void *p, SDL_Event *ev) {
	// vec3 front, up, right;
	// getv_upright(p, front, up, right);
	// player_move(p, up, PLAYER_MOVE_SPEED);

	// debug
	((Player*)p)->v.y = PLAYER_JUMP_SPEED;
	((Player*)p)->on_ground = false;
}
void player_down(void *p, SDL_Event *ev) {
	vec3 front, up, right;
	getv_upright(p, front, up, right);
	player_move(p, up, -PLAYER_MOVE_SPEED);
}
void player_rotate(void *p, SDL_Event *ev) {
	int dx = ev->motion.xrel;
	int dy = ev->motion.yrel;
	vec3 front, up, right;
	getv_upright(p, front, up, right);
	player_rotateHead(p, right, -dy * PLAYER_ROTATE_SENSI);

	vec3 fr = {1, 0, 0};
	glm_quat_rotatev(((Player*)p)->hrot, fr, fr);
	if (fabsf(glm_vec3_dot(fr, up)) > 0.99f) {
		// back
		player_rotateHead(p, right, dy * PLAYER_ROTATE_SENSI);
	}

	/* vertical first, then horizontal */
	player_rotateHead(p, up, -dx * PLAYER_ROTATE_SENSI);

	// body only rotate around up vec
	player_rotateBody(p, up, -dx * PLAYER_ROTATE_SENSI);
}

