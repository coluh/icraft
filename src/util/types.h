#ifndef _ICRAFT_util_types_h
#define _ICRAFT_util_types_h

/* very simple types */

// position, direction
typedef struct V3 {
	float x;
	float y;
	float z;
} V3;

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

#endif
