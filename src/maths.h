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

typedef union mat4
{
	f32 elements[4][4];
	f32 item[16];
} mat4;

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

inline vec4 Vec4(vec3 v, f32 f)
{
	vec4 result = { 0 };
	result.x = v.x;
	result.y = v.y;
	result.z = v.z;
	result.w = f;
	return result;
}

inline mat4 Mat4(f32 value)
{
	mat4 result = { 0 };
	result.elements[0][0] = value;
	result.elements[1][1] = value;
	result.elements[2][2] = value;
	result.elements[3][3] = value;
	return result;
}

inline vec2 Vec2Add(vec2 left, vec2 right)
{
	vec2 result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	return result;
}

inline vec3 Vec3Add(vec3 left, vec3 right)
{
	vec3 result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;
	return result;
}

inline vec4 Vec4Add(vec4 left, vec4 right)
{
	vec4 result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;
	result.w = left.w + right.w;
	return result;
}

inline vec2 Vec2Minus(vec2 left, vec2 right)
{
	vec2 result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	return result;
}

inline vec3 Vec3Minus(vec3 left, vec3 right)
{
	vec3 result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	return result;
}

inline vec4 Vec4Minus(vec4 left, vec4 right)
{
	vec4 result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	result.w = left.w - right.w;
	return result;
}

inline vec2 Vec2Multiply(vec2 left, vec2 right)
{
	vec2 result;
	result.x = left.x * right.x;
	result.y = left.y * right.y;
	return result;
}

inline vec3 Vec3Multiply(vec3 left, vec3 right)
{
	vec3 result;
	result.x = left.x * right.x;
	result.y = left.y * right.y;
	result.z = left.z * right.z;
	return result;
}

inline vec4 Vec4Multiply(vec4 left, vec4 right)
{
	vec4 result;
	result.x = left.x * right.x;
	result.y = left.y * right.y;
	result.z = left.z * right.z;
	result.w = left.w * right.w;
	return result;
}

inline vec2 Vec2Divide(vec2 left, vec2 right)
{
	vec2 result;
	result.x = left.x / right.x;
	result.y = left.y / right.y;
	return result;
}

inline vec3 Vec3Divide(vec3 left, vec3 right)
{
	vec3 result;
	result.x = left.x / right.x;
	result.y = left.y / right.y;
	result.z = left.z / right.z;
	return result;
}

inline vec4 Vec4Divide(vec4 left, vec4 right)
{
	vec4 result;
	result.x = left.x / right.x;
	result.y = left.y / right.y;
	result.z = left.z / right.z;
	result.w = left.w / right.w;
	return result;
}

inline vec4 Mat4MultiplyVec4(mat4 matrix, vec4 vector)
{
	vec4 result;
	for (s32 i = 0; i < 4; i++) {
		f32 sum = 0.0f;
		for (s32 j = 0; j < 4; j++) {
			sum += matrix.elements[i][j] * vector.elements[j];
		}
		result.elements[i] = sum;
	}
	return result;
}

inline mat4 Mat4Multiply(mat4 left, mat4 right)
{
	mat4 result;
	for (s32 j = 0; j < 4; ++j)
	{
		for (s32 i = 0; i < 4; ++i)
		{
			result.elements[i][j] = (left.elements[0][j] * right.elements[i][0] +
				left.elements[1][j] * right.elements[i][1] +
				left.elements[2][j] * right.elements[i][2] +
				left.elements[3][j] * right.elements[i][3]);
		}
	}
	return result;
}

inline f32 Vec3Length(vec3 v)
{
	f32 result = (f32)sqrt((v.x * v.x + v.y * v.y + v.z * v.z));
	return result;
}

inline vec3 Vec3Normalise(vec3 v)
{
	f32 length = Vec3Length(v);
	vec3 result = Vec3f(v.x / length, v.y / length, v.z / length);
	return result;
}

inline f32 Vec3Dot(vec3 v1, vec3 v2)
{
	f32 result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

inline vec3 Vec3Cross(vec3 v1, vec3 v2)
{
	vec3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

inline mat4 LookAt(vec3 eye, vec3 centre, vec3 up) {
	vec3 z_axis = Vec3Normalise(Vec3Minus(eye, centre));
	vec3 x_axis = Vec3Normalise(Vec3Cross(up, z_axis));
	vec3 y_axis = Vec3Normalise(Vec3Cross(z_axis, x_axis));

	mat4 result = Mat4(1.0f);

	result.elements[0][0] = x_axis.x;
	result.elements[0][1] = x_axis.y;
	result.elements[0][2] = x_axis.z;

	result.elements[1][0] = y_axis.x;
	result.elements[1][1] = y_axis.y;
	result.elements[1][2] = y_axis.z;

	result.elements[2][0] = z_axis.x;
	result.elements[2][1] = z_axis.y;
	result.elements[2][2] = z_axis.z;

	result.elements[0][3] = -centre.x;
	result.elements[1][3] = -centre.y;
	result.elements[2][3] = -centre.z;

	return result;
}

inline mat4 Viewport(s32 x, s32 y, s32 width, s32 height)
{
	mat4 result = Mat4(1.0f);

	result.elements[0][0] = width / 2.0f;
	result.elements[0][3] = x + width / 2.0f;

	result.elements[1][1] = height / 2.0f;
	result.elements[1][3] = y + height / 2.0f;

	result.elements[2][2] = 255.0f / 2.0f;
	result.elements[2][3] = 255.0f / 2.0f;

	return result;
}

inline mat4 Projection(f32 coeff)
{
	mat4 result = Mat4(1.0f);
	result.elements[3][2] = coeff;
	return result;
}

#endif 