#ifndef _ICRAFT_world_generation_generator_h
#define _ICRAFT_world_generation_generator_h

#include "../chunk.h"

typedef struct Generator {
	int seed;
} Generator;

Generator *generator_create(int seed);

// main terrain generation
void generator_generate(Generator *generator);

// fill chunk using generator
void generator_fill(Generator *generator, Chunk *chunk);

// debug generation
void generator_default(Chunk *chunk);

#endif
