#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "types.h"
#include "maths.h"

typedef struct Image
{
	s32 width, height, channels;
	u8 *buffer;
} Image;

Image *ReadFromTGA(const char* file_name);

vec3 SampleTexture(Image *texture, vec2 texcoord);

#endif