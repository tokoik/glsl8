#pragma once
#define SPHERE_H

/* OpenGL / GLSL */
#include "glsl.h"

/*
** 球の描画
*/
extern void sphere(double radius, int slices, int stacks, GLint tangent);
