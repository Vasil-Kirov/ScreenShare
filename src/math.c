#include "math.h"

float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

iv2 make_iv2(i32 x, i32 y)
{
	return (iv2){x, y};
}

v2 make_v2(float x, float y)
{
	return (v2){x, y};
}

v2 v2_add(v2 a, v2 b)
{
	return (v2){a.x+b.x, a.y+b.y};
}

v2 v2_sub(v2 a, v2 b)
{
	return (v2){a.x-b.x, a.y-b.y};
}

v2 v2_scale(v2 a, float f)
{
	return (v2){a.x*f, a.y*f};
}

v4 make_v4(float x, float y, float z, float w)
{
	return (v4){x, y, z, w};
}

v4 v4_scale(v4 a, float f)
{
	return (v4){a.x*f, a.y*f, a.z*f, a.w*f};
}

v4 v4_intersect(v4 a, v4 b)
{
	if(a.x < b.x) a.x = b.x;
	if(a.y < b.y) a.y = b.y;
	if(a.z > b.z) a.z = b.z;
	if(a.w > b.w) a.w = b.w;

	return a;
}

bool is_point_in_rect(v2 p, v4 r)
{
	return p.x >= r.x && p.y >= r.y && p.x <= r.z && p.y <= r.w;
}


