#include <engine_fixed.h>
#include <math.h>
#include <driver.h>
static inline int16_t to_fixed(float f)
{
    return f * ((int16_t)1 << 8);
}
static inline float fixed_to_float(int16_t f)
{
    return (float)f / ((int16_t)1 << 8);
}
static inline int16_t fixed_mult(int16_t a, int16_t b)
{
    return (int32_t)a * b >> 8;
}
static inline int16_t fixed_div(int16_t a, int16_t b)
{
    return (a << 8) / b;
}
static inline uint8_t fixed_to_int(int16_t f)
{
    return (uint8_t)(f>>8);//for tringle draw
}
void MultiplyMatVec(vec3d_fixed *vec_in, vec3d_fixed *vec_out, mat4_fixed *m)
{
    vec_out->x = fixed_mult(vec_in->x, m->m[0][0]) + fixed_mult(vec_in->y, m->m[1][0]) + fixed_mult(vec_in->z, m->m[2][0]) + m->m[3][0];
    vec_out->y = fixed_mult(vec_in->x, m->m[0][1]) + fixed_mult(vec_in->y, m->m[1][1]) + fixed_mult(vec_in->z, m->m[2][1]) + m->m[3][1];
    vec_out->z = fixed_mult(vec_in->x, m->m[0][2]) + fixed_mult(vec_in->y, m->m[1][2]) + fixed_mult(vec_in->z, m->m[2][2]) + m->m[3][2];
    // int16_t w = fixed_mult(vec_in->x , m->m[0][3]) + fixed_mult(vec_in->y , m->m[1][3]) + fixed_mult(vec_in->z, m->m[2][3]) + m->m[3][3];
}
void MultiplyMatVecProj(vec3d_fixed *vec_in, vec3d_fixed *vec_out, mat4_fixed *m)
{
    vec_out->x = fixed_mult(vec_in->x, m->m[0][0]);
    vec_out->y = fixed_mult(vec_in->y, m->m[1][1]);
    vec_out->z = fixed_mult(vec_in->z, m->m[2][2]) + m->m[3][2];
    // int16_t w = fixed_mult(vec_in->z, m->m[2][3]) + m->m[3][3];
    // if (fixed_to_float(w) != 0.00f)
    // {
    //     vec_out->x = fixed_div(vec_out->x, w);
    //     vec_out->y = fixed_div(vec_out->y, w);
    //     vec_out->z = fixed_div(vec_out->z, w);
    // }
}

float Fov = 120.0f;
float FovRad = (1.0f / tanf(Fov * 0.5 / 180.0f * PI));
fixed ThetaX = 0;
fixed ThetaY = 0;
void draw_cube_fixed(mat4_fixed *proj, MouseData *mice)
{
    static mat4_fixed matRotZ, matRotX;
    static float fThetaX = 0.0f;
    static float fThetaY = 0.0f;
    if (((*mice).status & 0x0F) == 0x09)
    {
        ThetaX -= fixed_mult(2560, ((*mice).position.x));
        ThetaY -= fixed_mult(2560, ((*mice).position.y));
    }
    if ((*mice).wheel != 0)
    {
        Fov += (float)mice->wheel * 2.0f;
        FovRad = 1.0f / tanf(Fov * 0.5 / 180.0f * PI);
        (*proj).m[0][0] = to_fixed(ffaspect_r * FovRad);
        (*proj).m[1][1] = to_fixed(FovRad);
    }
    fThetaX = fixed_to_float(ThetaX);
    fThetaY = fixed_to_float(ThetaY);
    //  Rotation
    matRotZ.m[0][0] = to_fixed(cosf(fThetaX));
    matRotZ.m[0][1] = to_fixed(sinf(fThetaX));
    matRotZ.m[1][0] = to_fixed(-sinf(fThetaX));
    matRotZ.m[1][1] = to_fixed(cosf(fThetaX));
    matRotZ.m[2][2] = 256;
    matRotZ.m[3][3] = 256;

    // Serial.println(to_fixed(-sinf(fThetaX)));

    // Rotation Y
    // matRotY.m[0][0] = cosf(fThetaX);
    // matRotY.m[0][2] = sinf(fThetaX);
    // matRotY.m[1][1] = 1;
    // matRotY.m[2][0] = -sinf(fThetaX);
    // matRotY.m[2][2] = cosf(fThetaX);
    // matRotY.m[3][3] = 1;

    // Rotation X
    matRotX.m[0][0] = 256;
    matRotX.m[1][1] = to_fixed(cosf(fThetaY));
    matRotX.m[1][2] = to_fixed(sinf(fThetaY));
    matRotX.m[2][1] = to_fixed(-sinf(fThetaY));
    matRotX.m[2][2] = to_fixed(cosf(fThetaY));
    matRotX.m[3][3] = 256;

    for (auto tri : MeshCubeFixed.tris)
    {
        triangle_fixed triCalc, triRotatedZ, triRotatedZX;

        // Rotate in Z-Axis
        MultiplyMatVec(&tri.p[0], &triRotatedZ.p[0], &matRotZ);
        MultiplyMatVec(&tri.p[1], &triRotatedZ.p[1], &matRotZ);
        MultiplyMatVec(&tri.p[2], &triRotatedZ.p[2], &matRotZ);
        // print matRotX
        //  Serial.println("matRotX");

        // Rotate in X-Axis
        MultiplyMatVec(&triRotatedZ.p[0], &triRotatedZX.p[0], &matRotX);
        MultiplyMatVec(&triRotatedZ.p[1], &triRotatedZX.p[1], &matRotX);
        MultiplyMatVec(&triRotatedZ.p[2], &triRotatedZX.p[2], &matRotX);

        //  //Rotate in Y-Axis
        // MultiplyMatVec(&triRotatedZX.p[0], &triRotatedZXY.p[0], &matRotY);
        // MultiplyMatVec(&triRotatedZX.p[1], &triRotatedZXY.p[1], &matRotY);
        // MultiplyMatVec(&triRotatedZX.p[2], &triRotatedZXY.p[2], &matRotY);

        MultiplyMatVecProj(&triRotatedZX.p[0], &triCalc.p[0], proj);
        MultiplyMatVecProj(&triRotatedZX.p[1], &triCalc.p[1], proj);
        MultiplyMatVecProj(&triRotatedZX.p[2], &triCalc.p[2], proj);
        for (byte i = 0; i < 3; i++)
        {
            triCalc.p[i].x += 256;
            triCalc.p[i].y += 256;
            triCalc.p[i].x = fixed_mult(triCalc.p[i].x, 10752);
            triCalc.p[i].y = fixed_mult(triCalc.p[i].y, 6144);
        }
        drawTriangle(fixed_to_float(triCalc.p[0].x), fixed_to_float(triCalc.p[0].y), fixed_to_float(triCalc.p[1].x), fixed_to_float(triCalc.p[1].y), fixed_to_float(triCalc.p[2].x), fixed_to_float(triCalc.p[2].y));
        // drawTriangle(triCalcfloat.p[0].x, triCalcfloat.p[0].y, triCalcfloat.p[1].x, triCalcfloat.p[1].y, triCalcfloat.p[2].x, triCalcfloat.p[2].y);
    }
}
mesh_fixed MeshCubeFixed =
    {
        // SOUTH
        (struct triangle_fixed){
            (struct vec3d_fixed){-256, 256, -256},
            (struct vec3d_fixed){-256, 256, 256},
            (struct vec3d_fixed){256, 256, 256}},
        (struct triangle_fixed){
            (struct vec3d_fixed){-256, 256, -256},
            (struct vec3d_fixed){256, 256, 256},
            (struct vec3d_fixed){256, 256, -256}},

        // EAST
        (struct triangle_fixed){
            (struct vec3d_fixed){256, 256, -256},
            (struct vec3d_fixed){256, 256, 256},
            (struct vec3d_fixed){256, -256, 256}},
        (struct triangle_fixed){
            (struct vec3d_fixed){256, 256, -256},
            (struct vec3d_fixed){256, -256, 256},
            (struct vec3d_fixed){256, -256, -256}},

        // NORTH
        (struct triangle_fixed){
            (struct vec3d_fixed){256, -256, -256},
            (struct vec3d_fixed){256, -256, 256},
            (struct vec3d_fixed){-256, -256, 256}},
        (struct triangle_fixed){
            (struct vec3d_fixed){256, -256, -256},
            (struct vec3d_fixed){-256, -256, 256},
            (struct vec3d_fixed){-256, -256, -256}},

        // WEST
        (struct triangle_fixed){
            (struct vec3d_fixed){-256, -256, -256},
            (struct vec3d_fixed){-256, -256, 256},
            (struct vec3d_fixed){-256, 256, 256}},
        (struct triangle_fixed){
            (struct vec3d_fixed){-256, -256, -256},
            (struct vec3d_fixed){-256, 256, 256},
            (struct vec3d_fixed){-256, 256, -256}},

        // TOP
        (struct triangle_fixed){
            (struct vec3d_fixed){-256, 256, 256},
            (struct vec3d_fixed){-256, -256, 256},
            (struct vec3d_fixed){256, -256, 256}},
        (struct triangle_fixed){
            (struct vec3d_fixed){-256, 256, 256},
            (struct vec3d_fixed){256, -256, 256},
            (struct vec3d_fixed){256, 256, 256}},

        // BOTTOM
        (struct triangle_fixed){
            (struct vec3d_fixed){-256, 256, -256},
            (struct vec3d_fixed){-256, -256, -256},
            (struct vec3d_fixed){256, -256, -256}},
        (struct triangle_fixed){
            (struct vec3d_fixed){-256, 256, -256},
            (struct vec3d_fixed){256, -256, -256},
            (struct vec3d_fixed){256, 256, -256}},

};
mat4_fixed _fstartE()
{
    // for (byte i = 0; i < 12; i++)///enable this if you want to have mesh represented with floats
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
    proj.m[1][1] = to_fixed(FovRad);
    proj.m[2][2] = to_fixed(fffar / (fffar - ffnear));
    proj.m[2][3] = 256;
    proj.m[3][2] = to_fixed((-fffar * ffnear) / (fffar - ffnear));
    proj.m[3][3] = 0;
    return proj;
}