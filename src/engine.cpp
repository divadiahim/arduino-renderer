#include <engine.h>
#include <math.h>
#include <driver.h>
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
float fTheta = 0;
void draw_cube(mat4 *proj)
{
    mat4 matRotZ, matRotX;
    fTheta += 1.0f * 0.05;

    // Rotation Z
    matRotZ.m[0][0] = cosf(fTheta);
    matRotZ.m[0][1] = sinf(fTheta);
    matRotZ.m[1][0] = -sinf(fTheta);
    matRotZ.m[1][1] = cosf(fTheta);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    // Rotation X
    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cosf(fTheta * 0.5f);
    matRotX.m[1][2] = sinf(fTheta * 0.5f);
    matRotX.m[2][1] = -sinf(fTheta * 0.5f);
    matRotX.m[2][2] = cosf(fTheta * 0.5f);
    matRotX.m[3][3] = 1;

    for (auto tri : MeshCube.tris)
    {
        triangle triCalc, tri_trans, triRotatedZ, triRotatedZX;

        // Rotate in Z-Axis
        MultiplyMatVec(&tri.p[0], &triRotatedZ.p[0], &matRotZ);
        MultiplyMatVec(&tri.p[1], &triRotatedZ.p[1], &matRotZ);
        MultiplyMatVec(&tri.p[2], &triRotatedZ.p[2], &matRotZ);

        // Rotate in X-Axis
        MultiplyMatVec(&triRotatedZ.p[0], &triRotatedZX.p[0], &matRotX);
        MultiplyMatVec(&triRotatedZ.p[1], &triRotatedZX.p[1], &matRotX);
        MultiplyMatVec(&triRotatedZ.p[2], &triRotatedZX.p[2], &matRotX);

        tri_trans = triRotatedZX;
        for (int i = 0; i < 3; i++)
        {
            tri_trans.p[i].z = tri.p[i].z + 0.1f;
        }
        MultiplyMatVec(&tri_trans.p[0], &triCalc.p[0], proj);
        MultiplyMatVec(&tri_trans.p[1], &triCalc.p[1], proj);
        MultiplyMatVec(&tri_trans.p[2], &triCalc.p[2], proj);

        // scale the view
        for (int i = 0; i < 3; i++)
        {
            triCalc.p[i].x += 1;
            triCalc.p[i].y += 1;
            triCalc.p[i].x *= 0.5 * (float)84;
            triCalc.p[i].y *= 0.5 * (float)48;
        }

        drawTriangle(triCalc.p[0].x, triCalc.p[0].y, triCalc.p[1].x, triCalc.p[1].y, triCalc.p[2].x, triCalc.p[2].y);
    }
}
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
            (struct vec3d){1.0f, 0.0f, 1.0f},
            (struct vec3d){0.0f, 0.0f, 0.0f},
            (struct vec3d){1.0f, 0.0f, 0.0f}},

};
mat4 _startE()
{

    mat4 proj;
    proj.m[0][0] = faspect_r * fFovRad;
    proj.m[1][1] = fFovRad;
    proj.m[2][2] = ffar / (ffar - fnear);
    proj.m[2][3] = 1.0f;
    proj.m[3][2] = (-ffar * fnear) / (ffar - fnear);
    proj.m[3][3] = 0.0f;
    return proj;
}
