#include "block.h"
#include <string.h>
#include "../../util/macros.h"
#include "../../util/log.h"

int block_getId(Block block) {
	return block & 0x0000FFFF;
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
