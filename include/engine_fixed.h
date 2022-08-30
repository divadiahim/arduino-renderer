#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <Arduino.h>
#include "mouse.h"
#define ffnear 3.5f
#define fffar 120.0f
#define ffaspect_r (float)48 / (float)84
typedef int16_t fixed;
extern float Fov;
extern float FovRad;
extern fixed ThetaX;
extern fixed ThetaY;
struct vec3d_fixed
{
    fixed x, y, z;
};
struct triangle_fixed
{
    vec3d_fixed p[3];
};
struct mesh_fixed
{
    triangle_fixed tris[12];
};
struct mat4_fixed
{
    fixed m[4][4] = {0};
};
extern mesh_fixed MeshCubeFixed;
mat4_fixed _fstartE();
void draw_cube_fixed(mat4_fixed *proj, MouseData *mice);

#endif