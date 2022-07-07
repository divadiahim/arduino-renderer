#include <engine_fixed.h>
#include <math.h>
#include <driver.h>

int32_t to_fixed(float f)
{
    return f * ((int32_t)1 << 16);
}
float fixed_to_float(int32_t f)
{
    return (float)f / ((int32_t)1 << 16);
}
inline int32_t fixed_mult(int32_t a, int32_t b)
{
    return (int64_t)a * (int64_t)b >> 16;
}
inline int32_t fixed_div(int32_t a, int32_t b)
{
    return ((int64_t)a<<16) / (int64_t)b;
}
void MultiplyMatVec(vec3d_fixed *vec_in, vec3d_fixed *vec_out, mat4_fixed *m)
{
    vec_out->x = fixed_mult(vec_in->x, m->m[0][0]) + fixed_mult(vec_in->y, m->m[1][0]) + fixed_mult(vec_in->z, m->m[2][0]) + m->m[3][0];
    vec_out->y = fixed_mult(vec_in->x, m->m[0][1]) + fixed_mult(vec_in->y, m->m[1][1]) + fixed_mult(vec_in->z, m->m[2][1]) + m->m[3][1];
    vec_out->z = fixed_mult(vec_in->x, m->m[0][2]) + fixed_mult(vec_in->y, m->m[1][2]) + fixed_mult(vec_in->z, m->m[2][2]) + m->m[3][2];
    int32_t w = fixed_mult(vec_in->z, m->m[2][3]) + m->m[3][3]; 
    if (!w)
    {
        vec_out->x= fixed_div(vec_out->x, w);
        vec_out->y = fixed_div(vec_out->y, w);
        vec_out->z = fixed_div(vec_out->z, w);


    }
}
void MultiplyMatVecProj(vec3d_fixed *vec_in, vec3d_fixed *vec_out, mat4_fixed *m)
{
    vec_out->x = vec_in->x * m->m[0][0];
    vec_out->y = vec_in->y * m->m[1][1];
    vec_out->z = vec_in->z * m->m[2][2] + m->m[3][2];
    float w = vec_in->z * m->m[2][3] + m->m[3][3];
    if (!w)
    {
        vec_out->x /= w;
        vec_out->y /= w;
        vec_out->z /= w;
    }
}

float Fov = 120.0f;
float FovRad = 1.0f / tanf(Fov * 0.5 / 180.0f * PI);
void draw_cube_fixed(mat4_fixed *proj)
{
    // mat4_fixed matRotZ, matRotX;

    // // Rotation Z
    // matRotZ.m[0][0] = cosf(fThetaX);
    // matRotZ.m[0][1] = sinf(fThetaX);
    // matRotZ.m[1][0] = -sinf(fThetaX);
    // matRotZ.m[1][1] = cosf(fThetaX);
    // matRotZ.m[2][2] = 1;
    // matRotZ.m[3][3] = 1;

    // // Rotation Y
    // // matRotY.m[0][0] = cosf(fThetaX);
    // // matRotY.m[0][2] = sinf(fThetaX);
    // // matRotY.m[1][1] = 1;
    // // matRotY.m[2][0] = -sinf(fThetaX);
    // // matRotY.m[2][2] = cosf(fThetaX);
    // // matRotY.m[3][3] = 1;

    // // Rotation X
    // matRotX.m[0][0] = 1;
    // matRotX.m[1][1] = cosf(fThetaY * 0.5f);
    // matRotX.m[1][2] = sinf(fThetaY * 0.5f);
    // matRotX.m[2][1] = -sinf(fThetaY * 0.5f);
    // matRotX.m[2][2] = cosf(fThetaY * 0.5f);
    // matRotX.m[3][3] = 1;

    for (auto tri : MeshCubeFixed.tris)
    {
        triangle_fixed triCalc, triRotatedZ, triRotatedZX;
        triangle triCalcfloat;

        // Rotate in Z-Axis
        // MultiplyMatVec(&tri.p[0], &triRotatedZ.p[0], &matRotZ);
        // MultiplyMatVec(&tri.p[1], &triRotatedZ.p[1], &matRotZ);
        // MultiplyMatVec(&tri.p[2], &triRotatedZ.p[2], &matRotZ);

        // // Rotate in X-Axis
        // MultiplyMatVec(&triRotatedZ.p[0], &triRotatedZX.p[0], &matRotX);
        // MultiplyMatVec(&triRotatedZ.p[1], &triRotatedZX.p[1], &matRotX);
        // MultiplyMatVec(&triRotatedZ.p[2], &triRotatedZX.p[2], &matRotX);

        //  //Rotate in X-Axis
        // MultiplyMatVec(&triRotatedZX.p[0], &triRotatedZXY.p[0], &matRotY);
        // MultiplyMatVec(&triRotatedZX.p[1], &triRotatedZXY.p[1], &matRotY);
        // MultiplyMatVec(&triRotatedZX.p[2], &triRotatedZXY.p[2], &matRotY);

        MultiplyMatVec(&tri.p[0], &triCalc.p[0], proj);
        MultiplyMatVec(&tri.p[1], &triCalc.p[1], proj);
        MultiplyMatVec(&tri.p[2], &triCalc.p[2], proj);
        // print to serial port triangle values
        Serial.print(triCalc.p[0].x); Serial.print(", "); Serial.print(triCalc.p[0].y); Serial.print(", "); Serial.println(triCalc.p[0].z);

        // scale the view
        //make tricalc floating
        for (byte i = 0; i < 3; i++)
        {
            triCalcfloat.p[i].x=fixed_to_float(triCalc.p[i].x);
            triCalcfloat.p[i].y=fixed_to_float(triCalc.p[i].y);
            triCalcfloat.p[i].z=fixed_to_float(triCalc.p[i].z);
        }     
        for (byte i = 0; i < 3; i++)
        {
            triCalcfloat.p[i].x += 1;
            triCalcfloat.p[i].y += 1;
            triCalcfloat.p[i].x *= 0.5 * (float)84;
            triCalcfloat.p[i].y *= 0.5 * (float)48;
        }
        //drawTriangle(fixed_to_float(triCalc.p[0].x), fixed_to_float(triCalc.p[0].y), fixed_to_float(triCalc.p[1].x), fixed_to_float(triCalc.p[1].y), fixed_to_float(triCalc.p[2].x), fixed_to_float(triCalc.p[2].y));
         drawTriangle(triCalcfloat.p[0].x, triCalcfloat.p[0].y, triCalcfloat.p[1].x, triCalcfloat.p[1].y, triCalcfloat.p[2].x, triCalcfloat.p[2].y);
    }
}
mesh_fixed MeshCubeFixed =
    {
        // SOUTH
        (struct triangle_fixed){
            (struct vec3d_fixed){-65536, 65536, -65536},
            (struct vec3d_fixed){-65536, 65536, 65536},
            (struct vec3d_fixed){65536, 65536, 65536}},
        (struct triangle_fixed){
            (struct vec3d_fixed){-65536, 65536, -65536},
            (struct vec3d_fixed){65536, 65536, 65536},
            (struct vec3d_fixed){65536, 65536, -65536}},

        // EAST
        (struct triangle_fixed){
            (struct vec3d_fixed){65536, 65536, -65536},
            (struct vec3d_fixed){65536, 65536, 65536},
            (struct vec3d_fixed){65536, -65536, 65536}},
        (struct triangle_fixed){
            (struct vec3d_fixed){65536, 65536, -65536},
            (struct vec3d_fixed){65536, -65536, 65536},
            (struct vec3d_fixed){65536, -65536, -65536}},

        // NORTH
        (struct triangle_fixed){
            (struct vec3d_fixed){65536, -65536, -65536},
            (struct vec3d_fixed){65536, -65536, 65536},
            (struct vec3d_fixed){-65536, -65536, 65536}},
        (struct triangle_fixed){
            (struct vec3d_fixed){65536, -65536, -65536},
            (struct vec3d_fixed){-65536, -65536, 65536},
            (struct vec3d_fixed){-65536, -65536, -65536}},

        // WEST
        (struct triangle_fixed){
            (struct vec3d_fixed){-65536, -65536, -65536},
            (struct vec3d_fixed){-65536, -65536, 65536},
            (struct vec3d_fixed){-65536, 65536, 65536}},
        (struct triangle_fixed){
            (struct vec3d_fixed){-65536, -65536, -65536},
            (struct vec3d_fixed){-65536, 65536, 65536},
            (struct vec3d_fixed){-65536, 65536, -65536}},

        // TOP
        (struct triangle_fixed){
            (struct vec3d_fixed){-65536, 65536, 65536},
            (struct vec3d_fixed){-65536, -65536, 65536},
            (struct vec3d_fixed){65536, -65536, 65536}},
        (struct triangle_fixed){
            (struct vec3d_fixed){-65536, 65536, 65536},
            (struct vec3d_fixed){65536, -65536, 65536},
            (struct vec3d_fixed){65536, 65536, 65536}},

        // BOTTOM
        (struct triangle_fixed){
            (struct vec3d_fixed){-65536, 65536, -65536},
            (struct vec3d_fixed){-65536, -65536, -65536},
            (struct vec3d_fixed){65536, -65536, -65536}},
        (struct triangle_fixed){
            (struct vec3d_fixed){-65536, 65536, -65536},
            (struct vec3d_fixed){65536, -65536, -65536},
            (struct vec3d_fixed){65536, 65536, -65536}},

};
mat4_fixed _fstartE()
{
    // for (byte i = 0; i < 12; i++)
    // {
    //     for (byte j = 0; j < 3; j++)
    //     {
    //         MeshCube_fixed.tris[i].p[j].x = to_fixed(MeshCube.tris[i].p[j].x);
    //         MeshCube_fixed.tris[i].p[j].y = to_fixed(MeshCube.tris[i].p[j].y);
    //         MeshCube_fixed.tris[i].p[j].z = to_fixed(MeshCube.tris[i].p[j].z);
    //     }
    // }

    mat4_fixed proj;
    proj.m[0][0] = to_fixed(ffaspect_r * FovRad);
    Serial.println(proj.m[0][0]);
    proj.m[1][1] = to_fixed(FovRad);
    proj.m[2][2] = to_fixed(fffar / (fffar - ffnear));
    proj.m[2][3] = 65536;
    proj.m[3][2] = to_fixed((-fffar * ffnear) / (fffar - ffnear));
    proj.m[3][3] = 0;
    for (byte z = 0; z < 12; z++)
        for (int i = 0; i < 3; i++)
        {
            MeshCubeFixed.tris[z].p[i].z += to_fixed(0.01f);
        }
    // print to the serial port the proj matrix
    // for (int i = 0; i < 4; i++)
    // {
    //     for (int j = 0; j < 4; j++)
    //     {
    //         Serial.print(i);
    //         Serial.print(" ");
    //         Serial.print(j);
    //         Serial.print(" ");
    //         Serial.print(proj.m[i][j]);
    //         Serial.print(" ");
    //         Serial.println();
    //     }
    // }
    return proj;
}