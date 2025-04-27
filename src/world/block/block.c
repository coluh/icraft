#include "block.h"
#include <string.h>
#include "../../util/props.h"
#include "../../util/log.h"

int block_getId(Block block) {
	return block & 0x0000FFFF;
}

// follow order of
// -z, z, -y, y, -x, x
static const int id_textures_map[][6] = {
	{0, 0, 0, 0, 0, 0},
	{2, 2, 3, 1, 2, 2},
	{3, 3, 3, 3, 3, 3},
	{4, 4, 4, 4, 4, 4},
};

void block_getTexture(Block block, int texture[]) {
	for (int i = 0; i < 6; i++) {
		texture[i] = id_textures_map[block_getId(block)][i];
	}
}

int block_getIdFaceTexture(int id, int face) {
	return id_textures_map[id][face];
}

void block_setId(Block *block, int id) {
	*block &= 0xFFFF0000;
	*block |= 0x0000FFFF & id;
}

static const char *id_block_map[] = {
	"air",
	"grass_block",
	"dirt",
	"stone",
};

int block_id_of(const char *name) {
	for (int i = 0; i < ARRLEN(id_block_map); i++) {
		if (strcmp(name, id_block_map[i]) == EQUAL) {
			return i;
		}
	}
	loge("no such block");
	return -1;
}
