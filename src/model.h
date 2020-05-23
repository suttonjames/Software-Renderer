#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "types.h"
#include "maths.h"

#include "stretchy_buffer.h"

typedef struct Model {
	vec3 *positions;
	vec2 *texcoords;
	vec3 *normals;
	s32 num_faces;
} Model;

Model *LoadModel(const char *file_name);
void FreeModel(Model *model);

#endif
