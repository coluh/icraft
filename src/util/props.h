#ifndef _ICRAFT_util_macros_h
#define _ICRAFT_util_macros_h

#include <stddef.h>

#define ARRLEN(arr) (sizeof(arr)/sizeof((arr)[0]))

#define EQUAL 0

#define IN_RECT(m, n, r) (((m)>=(r).x)&&((m)<((r).x+(r).w))&&((n)>=(r).y)&&((n)<((r).y+(r).h)))

#define POSITIVE_REMAINDER(a, b) (((a)%(b)>0)?((a)%(b)):((a)%(b)+(b)))
#define ROUND_DOWN_BY(x, m) ((x)-POSITIVE_REMAINDER(x,m))

#define UNPACK3(arr) (arr)[0],(arr)[1],(arr)[2]
#define UNPACK_RGBA(c) (c).r,(c).g,(c).b,(c).a
#define UNPACK_RECT(r) (r).x,(r).y,(r).w,(r).h
#define UNPACK_RECT_SHRINK(r, a) (r).x+(a),(r).y+(a),(r).w-2*(a),(r).h-2*(a)
#define PACK_RGBA(c, rv, gv, bv, av) (c).r=(rv);(c).g=(gv);(c).b=(bv);(c).a=(av);

// used for an array of struct pointer, to find max INT field of structs
#define MAX_FIELD_INT_P(strp_arr, str_name, field, count) max_field_int_p((void**)strp_arr, offsetof(str_name, field), count)
// used for an array of struct pointer, to get sum of INT field of structs
#define SUM_FIELD_INT_P(strp_arr, str_name, field, count) sum_field_int_p((void**)strp_arr, offsetof(str_name, field), count)

#define FORR(n) for (int i = 0; i < n; i++)

void props_init();

// [min, max)
int rand_int(int min, int max);

int max_field_int_p(void *strp_arr[], int field_offset, int count);
int sum_field_int_p(void *strp_arr[], int field_offset, int count);

#endif
