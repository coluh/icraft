#include "render_3d.h"
#include "../world/chunk.h"
#include "../player/camera.h"
#include "gl.h"

static void threed_renderChunk(Chunk *chunk) {
	gl_setModel(chunk->model);
	gl_renderTriangles(chunk->VAO, chunk->vertex_count);
}

void threed_renderChunks(Camera *camera) {

	gl_begin3d();
	gl_setView(camera->view);
	gl_setProj(camera->proj);
	gl_bindBlocksTexture();

	chunks_foreach(threed_renderChunk);
}
