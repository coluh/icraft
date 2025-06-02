#include "particle.h"
#include "../util/mem.h"
#include "../util/props.h"
#include "../../third_party/cglm/include/cglm/vec3.h"
#include "../render/gl.h"
#include "../render/texture.h"
#include "../camera/camera.h"
#include "../game.h"
#include "../world/world.h"
#include "../render/resource.h"

extern Game g;

static struct ParticlePool {
	Particle *particles;
	int capacity;
} pool;

Particle *particle_spawn(float x, float y, float z, int texture_index) {
	for (int i = 0; i < pool.capacity; i++) {
		Particle *p = &pool.particles[i];
		if (p->alive) {
			continue;
		}
		p->alive = true;
		p->pos = (V3){x, y, z};
		p->v.x = rand_float(-2.0f, 2.0f);
		p->v.y = rand_float(0.0f, 5.0f);
		p->v.z = rand_float(-2.0f, 2.0f);
		p->lifetime = 2.0f;
		p->age = 0.0f;
		p->size = rand_float(0.1f, 0.2f);
		p->texture_index = texture_index;
		return p;
	}
	if (pool.capacity == 0) {
		pool.capacity = 32;
		pool.particles = zalloc(pool.capacity, sizeof(Particle));
	} else {
		pool.capacity *= 2;
		pool.particles = rezalloc(pool.particles, pool.capacity, sizeof(Particle));
	}
	return particle_spawn(x, y, z, texture_index);
}

void particle_update() {
	for (int i = 0; i < pool.capacity; i++) {
		Particle *p = &pool.particles[i];
		if (p->alive) {
			p->pre_pos = p->pos;
			p->v.y += g.world->g * g.update_delta;
			p->pos.x += p->v.x * g.update_delta;
			p->pos.y += p->v.y * g.update_delta;
			p->pos.z += p->v.z * g.update_delta;
			p->age += g.update_delta;
			if (p->age >= p->lifetime) {
				p->alive = false;
			}
		}
	}
}

void particle_render(Camera *camera, float alpha) {
	for (int i = 0; i < pool.capacity; i++) {
		Particle *p = &pool.particles[i];
		if (p->alive) {
			vec3 position;
			glm_vec3_lerp((float*)&p->pre_pos, (float*)&p->pos, alpha, position);

			glUseProgram(g.res->shaders.particle);
			glBindTexture(GL_TEXTURE_2D, g.res->textures.blocks);
			float uv[2];
			texture_blockUVoffset(p->texture_index, uv);
			glUniform2fv(g.res->shaders.particle_location.uv_offset, 1, uv);
			glUniform3fv(g.res->shaders.particle_location.particlePos, 1, position);
			glUniform1f(g.res->shaders.particle_location.size, p->size);
			glUniformMatrix4fv(g.res->shaders.particle_location.view, 1, GL_FALSE, (float*)camera->view);
			glUniformMatrix4fv(g.res->shaders.particle_location.proj, 1, GL_FALSE, (float*)camera->proj);

			glBindVertexArray(g.res->meshes.particleVAO);
			glDrawArrays(GL_TRIANGLES, 0, g.res->meshes.particleVAO_count);
		}
	}
}
