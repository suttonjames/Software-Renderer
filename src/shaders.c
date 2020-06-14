#include "shaders.h"

vec4 VertexShader(s32 vertex, void *varyings_, void *uniforms_)
{
	Varyings *varyings = (Varyings *)varyings_;
	Uniforms *uniforms = (Uniforms *)uniforms_;

	vec3 in_position = varyings->in_positions[vertex];
	vec2 in_texcoord = varyings->in_texcoords[vertex];
	mat4 mvp = uniforms->mvp;

	vec2 *out_texcoord = &varyings->out_texcoords[vertex];
	*out_texcoord = in_texcoord;

	vec4 position = Vec4(in_position, 1.0f);
	vec4 clip_coord = Mat4MultiplyVec4(mvp, position);

	return clip_coord;
}

vec3 FragmentShader(void *varyings_, void *uniforms_)
{
	Varyings *varyings = (Varyings *)varyings_;
	Uniforms *uniforms = (Uniforms *)uniforms_;

	vec2 in_texcoord = varyings->in_texcoord;

	mat4 mvp = uniforms->mvp;
	mat4 mvp_inverse = uniforms->mvp_inverse;
	vec3 light = uniforms->light;
	Image *diffuse_map = uniforms->diffuse_map;
	Image *normal_map = uniforms->normal_map;
	Image *specular_map = uniforms->specular_map;

	// transfor normal
	vec3 normal = SampleTexture(normal_map, in_texcoord);
	normal.x = normal.r / 255.0f * 2.0f - 1.0f;
	normal.y = normal.g / 255.0f * 2.0f - 1.0f;
	normal.z = normal.b / 255.0f * 2.0f - 1.0f;
	vec4 normal_4f = Vec4(normal, 1.0f);
	normal_4f = Mat4MultiplyVec4(mvp, normal_4f);
	normal = Vec3Normalise(Vec3(normal_4f));

	// transform light
	vec4 light_4f = Vec4(light, 1.0f);
	light_4f = Mat4MultiplyVec4(mvp, light_4f);
	light = Vec3Normalise(Vec3(light_4f));

	// reflected = 2 * normal * dot(normal, light) - light
	float intensity = Vec3Dot(normal, light);
	vec3 reflected = Vec3Scale(normal, intensity * 2.0f);
	reflected = Vec3Normalise(Vec3Minus(reflected, light));

	// specular factor
	vec3 spec = SampleTexture(specular_map, in_texcoord);
	float specular = spec.b;
	float base = max(reflected.z, 0.0f);
	specular = (float)pow(base, specular);

	// diffuse factor
	float diffuse = max(intensity, 0.0f);

	vec3 colour = SampleTexture(diffuse_map, in_texcoord);
	colour.r = 5.0f + colour.r * (diffuse + 0.6f * specular);
	colour.g = 5.0f + colour.g * (diffuse + 0.6f * specular);
	colour.b = 5.0f + colour.b * (diffuse + 0.6f * specular);
	colour.r = min(255.0f, colour.r);
	colour.g = min(255.0f, colour.g);
	colour.b = min(255.0f, colour.b);

	return colour;
}