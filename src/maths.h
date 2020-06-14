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

inline vec3 Vec3(vec4 v)
{
	vec3 result = { 0 };
	result.x = v.x / v.w;
	result.y = v.y / v.w;
	result.z = v.z / v.w;
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

inline mat4 Mat4MultiplyFloat(mat4 m, f32 f)
{
	for (s32 j = 0; j < 4; ++j) {
		for (s32 i = 0; i < 4; ++i) {
			m.elements[i][j] *= f;
		}
	}
	return m;
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

inline vec3 Vec3Scale(vec3 v, f32 f)
{
	vec3 result;
	result.x = v.x * f;
	result.y = v.y * f;
	result.z = v.z * f;
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

inline mat4 Mat4Inverse(mat4 m)
{
	f32 coef00 = m.elements[2][2] * m.elements[3][3] - m.elements[3][2] * m.elements[2][3];
	f32 coef02 = m.elements[1][2] * m.elements[3][3] - m.elements[3][2] * m.elements[1][3];
	f32 coef03 = m.elements[1][2] * m.elements[2][3] - m.elements[2][2] * m.elements[1][3];
	f32 coef04 = m.elements[2][1] * m.elements[3][3] - m.elements[3][1] * m.elements[2][3];
	f32 coef06 = m.elements[1][1] * m.elements[3][3] - m.elements[3][1] * m.elements[1][3];
	f32 coef07 = m.elements[1][1] * m.elements[2][3] - m.elements[2][1] * m.elements[1][3];
	f32 coef08 = m.elements[2][1] * m.elements[3][2] - m.elements[3][1] * m.elements[2][2];
	f32 coef10 = m.elements[1][1] * m.elements[3][2] - m.elements[3][1] * m.elements[1][2];
	f32 coef11 = m.elements[1][1] * m.elements[2][2] - m.elements[2][1] * m.elements[1][2];
	f32 coef12 = m.elements[2][0] * m.elements[3][3] - m.elements[3][0] * m.elements[2][3];
	f32 coef14 = m.elements[1][0] * m.elements[3][3] - m.elements[3][0] * m.elements[1][3];
	f32 coef15 = m.elements[1][0] * m.elements[2][3] - m.elements[2][0] * m.elements[1][3];
	f32 coef16 = m.elements[2][0] * m.elements[3][2] - m.elements[3][0] * m.elements[2][2];
	f32 coef18 = m.elements[1][0] * m.elements[3][2] - m.elements[3][0] * m.elements[1][2];
	f32 coef19 = m.elements[1][0] * m.elements[2][2] - m.elements[2][0] * m.elements[1][2];
	f32 coef20 = m.elements[2][0] * m.elements[3][1] - m.elements[3][0] * m.elements[2][1];
	f32 coef22 = m.elements[1][0] * m.elements[3][1] - m.elements[3][0] * m.elements[1][1];
	f32 coef23 = m.elements[1][0] * m.elements[2][1] - m.elements[2][0] * m.elements[1][1];

	vec4 fac0 = { coef00, coef00, coef02, coef03 };
	vec4 fac1 = { coef04, coef04, coef06, coef07 };
	vec4 fac2 = { coef08, coef08, coef10, coef11 };
	vec4 fac3 = { coef12, coef12, coef14, coef15 };
	vec4 fac4 = { coef16, coef16, coef18, coef19 };
	vec4 fac5 = { coef20, coef20, coef22, coef23 };

	vec4 vec0 = { m.elements[1][0], m.elements[0][0], m.elements[0][0], m.elements[0][0] };
	vec4 vec1 = { m.elements[1][1], m.elements[0][1], m.elements[0][1], m.elements[0][1] };
	vec4 vec2 = { m.elements[1][2], m.elements[0][2], m.elements[0][2], m.elements[0][2] };
	vec4 vec3 = { m.elements[1][3], m.elements[0][3], m.elements[0][3], m.elements[0][3] };

	vec4 inv0 = Vec4Add(Vec4Minus(Vec4Multiply(vec1, fac0), Vec4Multiply(vec2, fac1)), Vec4Multiply(vec3, fac2));
	vec4 inv1 = Vec4Add(Vec4Minus(Vec4Multiply(vec0, fac0), Vec4Multiply(vec2, fac3)), Vec4Multiply(vec3, fac4));
	vec4 inv2 = Vec4Add(Vec4Minus(Vec4Multiply(vec0, fac1), Vec4Multiply(vec1, fac3)), Vec4Multiply(vec3, fac5));
	vec4 inv3 = Vec4Add(Vec4Minus(Vec4Multiply(vec0, fac2), Vec4Multiply(vec1, fac4)), Vec4Multiply(vec2, fac5));

	vec4 sign_a = { +1, -1, +1, -1 };
	vec4 sign_b = { -1, +1, -1, +1 };

	mat4 inverse;
	for (u32 i = 0; i < 4; ++i) {
		inverse.elements[0][i] = inv0.elements[i] * sign_a.elements[i];
		inverse.elements[1][i] = inv1.elements[i] * sign_b.elements[i];
		inverse.elements[2][i] = inv2.elements[i] * sign_a.elements[i];
		inverse.elements[3][i] = inv3.elements[i] * sign_b.elements[i];
	}

	vec4 row0 = { inverse.elements[0][0], inverse.elements[1][0], inverse.elements[2][0], inverse.elements[3][0] };
	vec4 m0 = { m.elements[0][0], m.elements[0][1], m.elements[0][2], m.elements[0][3] };
	vec4 dot0 = Vec4Multiply(m0, row0);
	f32 dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

	f32 one_over_determinant = 1 / dot1;

	return Mat4MultiplyFloat(inverse, one_over_determinant);
}

inline mat4 Mat4Transpose(mat4 m)
{
	mat4 result;
	for (s32 j = 0; j < 4; ++j) {
		for (s32 i = 0; i < 4; ++i) {
			result.elements[i][j] = m.elements[j][i];
		}
	}
	return result;
}

inline mat4 Mat4InverseTranspose(mat4 m)
{
	mat4 result;
	result = Mat4Inverse(m);
	result = Mat4Transpose(m);
	return result;
}

#endif 