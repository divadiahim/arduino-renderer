#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <Arduino.h>
struct vec3d
{
    float x, y, z;
};
struct triangle
{
    vec3d p[3];
};
struct mesh
{
    triangle tris[12];
};

struct mat4
{
    float m[4][4] = {0};
};



void _startE();
void MultiplyMatVec(vec3d *vec_in, vec3d *vec_out, mat4 *m);

#endif