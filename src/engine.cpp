#include <engine.h>
#include <math.h>
void MultiplyMatVec(vec3d *vec_in, vec3d *vec_out, mat4 *m)
{
    vec_out->x = vec_in->x * m->m[0][0] + vec_in->y * m->m[1][0] + vec_in->z * m->m[2][0] + m->m[3][0];
    vec_out->y = vec_in->x * m->m[0][1] + vec_in->y * m->m[1][1] + vec_in->z * m->m[2][1] + m->m[3][1];
    vec_out->z = vec_in->x * m->m[0][2] + vec_in->y * m->m[1][2] + vec_in->z * m->m[2][2] + m->m[3][2];
    float w = vec_in->x * m->m[0][3] + vec_in->y * m->m[1][3] + vec_in->z * m->m[2][3] + m->m[3][3];
    if (!w)
    {
        vec_out->x /= w;
        vec_out->y /= w;
        vec_out->z /= w;
    }
}
void _startE()
{
    mesh MeshCube =
        {
            // SOUTH
            (struct triangle){
                (struct vec3d){0.0f, 0.0f, 0.0f},
                (struct vec3d){0.0f, 1.0f, 0.0f},
                (struct vec3d){1.0f, 1.0f, 0.0f}},
            (struct triangle){
                (struct vec3d){0.0f, 0.0f, 0.0f},
                (struct vec3d){1.0f, 1.0f, 0.0f},
                (struct vec3d){1.0f, 0.0f, 0.0f}},

            // EAST
            (struct triangle){
                (struct vec3d){1.0f, 0.0f, 0.0f},
                (struct vec3d){1.0f, 1.0f, 0.0f},
                (struct vec3d){1.0f, 1.0f, 1.0f}},
            (struct triangle){
                (struct vec3d){1.0f, 0.0f, 0.0f},
                (struct vec3d){1.0f, 1.0f, 1.0f},
                (struct vec3d){1.0f, 0.0f, 1.0f}},

            // NORTH
            (struct triangle){
                (struct vec3d){1.0f, 0.0f, 1.0f},
                (struct vec3d){1.0f, 1.0f, 1.0f},
                (struct vec3d){0.0f, 1.0f, 1.0f}},
            (struct triangle){
                (struct vec3d){1.0f, 0.0f, 1.0f},
                (struct vec3d){0.0f, 1.0f, 1.0f},
                (struct vec3d){0.0f, 0.0f, 1.0f}},

            // WEST
            (struct triangle){
                (struct vec3d){0.0f, 0.0f, 1.0f},
                (struct vec3d){0.0f, 1.0f, 1.0f},
                (struct vec3d){0.0f, 1.0f, 0.0f}},
            (struct triangle){
                (struct vec3d){0.0f, 0.0f, 1.0f},
                (struct vec3d){0.0f, 1.0f, 0.0f},
                (struct vec3d){0.0f, 0.0f, 0.0f}},

            // TOP
            (struct triangle){
                (struct vec3d){0.0f, 1.0f, 0.0f},
                (struct vec3d){0.0f, 1.0f, 1.0f},
                (struct vec3d){1.0f, 1.0f, 1.0f}},
            (struct triangle){
                (struct vec3d){0.0f, 1.0f, 0.0f},
                (struct vec3d){1.0f, 1.0f, 1.0f},
                (struct vec3d){1.0f, 1.0f, 0.0f}},

            // BOTTOM
            (struct triangle){
                (struct vec3d){1.0f, 0.0f, 1.0f},
                (struct vec3d){0.0f, 0.0f, 1.0f},
                (struct vec3d){0.0f, 0.0f, 0.0f}},
            (struct triangle){
                (struct vec3d){0.0f, 1.0f, 0.0f},
                (struct vec3d){0.0f, 0.0f, 0.0f},
                (struct vec3d){1.0f, 0.0f, 0.0f}},

        };

    float fnear = .1f;
    float ffar = 1000.0f;
    float fFov = 90.0f;
    float faspect_r = (float)48 / (float)84;
    float fFovRad = 1.0f / tanf(fFov * 0.5 / 180.0f * PI);

    mat4 proj;
    proj.m[0][0] = faspect_r * fFovRad;
    proj.m[1][1] = fFovRad;
    proj.m[2][2] = ffar / (ffar - fnear);
    proj.m[2][3] = 1.0f;
    proj.m[3][2] = -(ffar * fnear) / (ffar - fnear);
    proj.m[3][3] = 0.0f;
}
