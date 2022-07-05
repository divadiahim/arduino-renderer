#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <Arduino.h>
#include "mouse.h"
#define fnear  0.1f
#define ffar  100.0f
#define faspect_r  (float)48 / (float)84
extern float fFov;
extern float fFovRad;
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

extern mesh MeshCube;

mat4 _startE();
void draw_cube(mat4 *proj, MouseData *mice);
#endif