#ifndef MATHS_H
#define MATHS_H

#include <math.h>

#include "types.h"

typedef struct vec2 {
	struct {
		f32 x, y;
	};
	struct {
		f32 u, v;
	};
	f32 elements[2];
} vec2;

typedef union vec3 {
	struct {
		f32 x, y, z;
	};
	struct {
		f32 u, v, w;
	};
	struct {
		f32 r, g, b;
	};
	f32 elements[3];
} vec3;

typedef union vec4 {
	struct {
		union {
			vec3 xyz;
			struct {
				f32 x, y, z;
			};
		};
		f32 w;
	};
	struct {
		union {
			vec3 rgb;
			struct {
				f32 r, g, b;
			};
		};
		f32 a;
	};
	f32 elements[4];
} vec4;

inline vec2 Vec2i(s32 x, s32 y)
{
	vec2 result = { 0 };
	result.x = (f32)x;
	result.y = (f32)y;
	return result;
}

inline vec2 Vec2f(f32 x, f32 y)
{
	vec2 result = { 0 };
	result.x = x;
	result.y = y;
	return result;
}

inline vec3 Vec3i(s32 x, s32 y, s32 z)
{
	vec3 result = { 0 };
	result.x = (f32)x;
	result.y = (f32)y;
	result.z = (f32)z;
	return result;
}

inline vec3 Vec3f(f32 x, f32 y, f32 z)
{
	vec3 result = { 0 };
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}

inline vec4 Vec4i(s32 x, s32 y, s32 z, s32 w)
{
	vec4 result = { 0 };
	result.x = (f32)x;
	result.y = (f32)y;
	result.z = (f32)z;
	result.w = (f32)w;
	return result;
}

inline vec4 Vec4f(f32 x, f32 y, f32 z, f32 w)
{
	vec4 result = { 0 };
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	return result;
}

inline vec2 Vec2Minus(vec2 left, vec2 right)
{
	vec2 result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	return result;
}
#endif 