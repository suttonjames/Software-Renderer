#ifndef SHADERS_H
#define SHADERS_H

#include "types.h"
#include "maths.h"

#include "image.h"

typedef struct Varyings {
	// input vertex shader
	vec3 in_positions[3];
	vec2 in_texcoords[3];

	// output vertex shader
	vec2 out_texcoords[3];

	// input fragment shader
	vec2 in_texcoord;
} Varyings;

typedef struct Uniforms {
	mat4 mvp;
	mat4 mvp_inverse;
	vec3 light;
	Image *diffuse_map;
	Image *normal_map;
	Image *specular_map;
} Uniforms;

typedef struct Program {
	void *varyings;
	void *uniforms;
} Program;

vec4 VertexShader(s32 vertex, void *varyings, void *uniforms);
vec3 FragmentShader(void *varyings, void *uniforms);

#endif