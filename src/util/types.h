#ifndef _ICRAFT_util_types_h
#define _ICRAFT_util_types_h

/* very simple types */

// position, direction
typedef struct V3 {
	float x;
	float y;
	float z;
} V3;

// uv
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

// reference to pool entity
typedef struct PoolHandle {
	unsigned int index;
	unsigned int generation;
} PoolHandle;

// standard vertex
typedef struct Vertex {
	V3 pos;
	V3 norm;
	V2 uv;
} Vertex;

// index to faces
typedef enum FaceIndex {
	// -z, z, -y, y, -x, x
	Face_BACK, Face_FRONT, Face_BOTTOM, Face_TOP, Face_LEFT, Face_RIGHT,
} FaceIndex;


#endif
