#ifndef _ICRAFT_world_block_block_h
#define _ICRAFT_world_block_block_h

#include <stdint.h>

#define BLOCK_TEXTURE_SIZE 16
#define BLOCK_TEXTURE_ROW_COUNT 16

#define ID_AIR 0

// id in lower 16 bits
typedef uint32_t Block;

int block_getId(Block block);
void block_setId(Block *block, int id);

int block_id_of(const char *name);

#endif
