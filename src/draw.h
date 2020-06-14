#ifndef DRAW_H
#define DRAW_H

#include "platform.h"
#include "shaders.h"

void Draw(Backbuffer *buffer, Program *program, mat4 viewport);

#endif