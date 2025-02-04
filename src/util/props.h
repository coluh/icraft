#ifndef _ICRAFT_util_macros_h
#define _ICRAFT_util_macros_h

#define ARRLEN(arr) (sizeof(arr)/sizeof((arr)[0]))

#define EQUAL 0

#define POSITIVE_REMAINDER(a, b) (((a)%(b)>0)?((a)%(b)):((a)%(b)+(b)))
#define ROUND_DOWN_BY(x, m) ((x)-POSITIVE_REMAINDER(x,m))

#define UNPACK3(arr) (arr)[0],(arr)[1],(arr)[2]

void props_init();

// [min, max)
int rand_int(int min, int max);

#endif
