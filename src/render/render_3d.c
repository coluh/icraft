#include "render_3d.h"
#include "../game.h"
#include "resource.h"
#include "../world/chunk.h"
#include "../camera/camera.h"
#include "gl.h"

extern Game g;

static void threed_renderChunk(Chunk *chunk) {
	glUniformMatrix4fv(g.res->shaders.basic_location.model, 1, GL_FALSE, (float*)chunk->model);
	glBindVertexArray(chunk->VAO);
	glDrawArrays(GL_TRIANGLES, 0, chunk->vertex_count);
}

void threed_renderChunks(Camera *camera, World *world) {

	glActiveTexture(GL_TEXTURE0);  // no need, because tex0 is activated by default
	glBindTexture(GL_TEXTURE_2D, g.res->textures.blocks);

	for (ChunkNode *p = world->chunks; p != NULL; p = p->next) {
		threed_renderChunk(p->chunk);
	}
}
