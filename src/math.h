#pragma once
#include "vtypes.h"
#include <stdbool.h>

typedef struct v2 v2;
typedef struct v3 v3;
typedef struct v4 v4;

typedef struct iv2 iv2;
typedef struct iv3 iv3;
typedef struct iv4 iv4;

struct v2 {
	float x;
	float y;
};

struct v3 {
	float x;
	float y;
	float z;
};

struct v4 {
	float x;
	float y;
	float z;
	float w;
};

struct iv2 {
	i32 x;
	i32 y;
};

struct iv3 {
	i32 x;
	i32 y;
	i32 z;
};

struct iv4 {
	i32 x;
	i32 y;
	i32 z;
	i32 w;
};

float lerp(float a, float b, float t);

bool is_point_in_rect(v2 p, v4 r);

/* iv2 */
iv2 make_iv2(i32 x, i32 y);

/* v2 */
v2 make_v2(float x, float y);
v2 v2_add(v2 a, v2 b);
v2 v2_sub(v2 a, v2 b);
v2 v2_scale(v2 a, float f);

/* v4 */
v4 make_v4(float x, float y, float z, float w);
v4 v4_scale(v4 a, float f);
v4 v4_intersect(v4 a, v4 b);

