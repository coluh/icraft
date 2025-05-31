#ifndef _ICRAFT_util_types_h
#define _ICRAFT_util_types_h

/* very simple types */

// position, direction
typedef struct V3 {
	float x;
	float y;
	float z;
} V3;

typedef struct V2 {
	float x;
	float y;
} V2;

// block position
typedef struct IV3 {
	int x;
	int y;
	int z;
} IV3;

// cube body
typedef struct Body {
	float x, y, z;
	float w, h, d;
} Body;

typedef struct PoolHandle {
	unsigned int index;
	unsigned int generation;
} PoolHandle;

typedef struct Vertex {
	V3 pos;
	V3 norm;
	V2 uv;
} Vertex;

#endif
