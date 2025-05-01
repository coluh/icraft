#ifndef _ICRAFT_world_generation_generator_h
#define _ICRAFT_world_generation_generator_h

#include "../chunk.h"

// generate terrain
// fill the blocks field, with grass*1 dirt*3 and stone*28
// fill Chunk->blocks
void generator_default(Chunk *chunk);

#endif
