#ifndef PLATFORM_H
#define PLATFORM_H

#include <windows.h>

#include "types.h"
#include "maths.h"

#include "shaders.h"

typedef struct Backbuffer {
	s32 width;
	s32 height;
	void *memory;
	BITMAPINFO bitmapInfo;
	f32 *zbuffer;
} Backbuffer;

typedef struct Platform {
	b32 running;
	Backbuffer backbuffer;
	mat4 viewport;
	Program program;
} Platform;

// remove globals in future
static Platform platform;

#endif