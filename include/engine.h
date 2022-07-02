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
extern mesh MeshCube;
extern mat4 proj;
extern float fTheta;
#define fnear  0.1f
#define ffar  100.0f
#define fFov  115.0f
#define faspect_r  (float)48 / (float)84
#define fFovRad  1.0f / tanf(fFov * 0.5 / 180.0f * PI)

mat4 _startE();
void draw_cube(mat4 *proj);
void MultiplyMatVec(vec3d *vec_in, vec3d *vec_out, mat4 *m);
#endif