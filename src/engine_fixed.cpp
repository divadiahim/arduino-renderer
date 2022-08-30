#include <engine_fixed.h>
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
    return (uint8_t)(f >> 8); // for tringle draw
}
int16_t fpsin(int16_t i)
{
    /* Convert (signed) input to a value between 0 and 8192. (8192 is pi/2, which is the region of the curve fit). */
    /* ------------------------------------------------------------------- */
    i <<= 1;
    uint8_t c = i < 0; // set carry for output pos/neg

    if (i == (i | 0x4000)) // flip input value to corresponding value in range [0..8192)
        i = (1 << 15) - i;
    i = (i & 0x7FFF) >> 1;
    /* ------------------------------------------------------------------- */

    /* The following section implements the formula:
     = y * 2^-n * ( A1 - 2^(q-p)* y * 2^-n * y * 2^-n * [B1 - 2^-r * y * 2^-n * C1 * y]) * 2^(a-q)
    Where the constants are defined as follows:
    */
    // enum userchoice
    // {
    //     Toyota = 1,
    //     Lamborghini,
    //     Ferrari,
    //     Holden,
    //     RangeRover
    // };

    enum
    {
        A1 = 3370945099UL,
        b1 = 2746362156UL,
        C1 = 292421UL
    };
    enum
    {
        n = 13,
        p = 32,
        q = 31,
        r = 3,
        a = 12
    };

    uint32_t y = (C1 * ((uint32_t)i)) >> n;
    y = b1 - (((uint32_t)i * y) >> r);
    y = (uint32_t)i * (y >> n);
    y = (uint32_t)i * (y >> n);
    y = A1 - (y >> (p - q));
    y = (uint32_t)i * (y >> n);
    y = (y + (1UL << (q - a - 1))) >> (q - a); // Rounding

    return c ? -y : y;
}
#define fpcos(i) fpsin((int16_t)(((uint16_t)(i)) + 8192U))
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
vec3d_fixed CameraPos = {0, 0, 0};
void draw_cube_fixed(mat4_fixed *proj, MouseData *mice)
{
    static mat4_fixed matRotZ, matRotX, matt;
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
    // ThetaX += 1;
    // ThetaY -= 1;
    fThetaX = fixed_to_float(ThetaX);
    fThetaY = fixed_to_float(ThetaY);

    // Rotation
    matRotZ.m[0][0] = to_fixed(cosf(fThetaX));
    matRotZ.m[0][1] = to_fixed(sinf(fThetaX));
    matRotZ.m[1][0] = to_fixed(-sinf(fThetaX));
    matRotZ.m[1][1] = to_fixed(cosf(fThetaX));
    matRotZ.m[2][2] = 256;
    matRotZ.m[3][3] = 256;

    // matRotZ.m[0][0] = fpcos(ThetaX<<4)>>4;
    // matRotZ.m[0][1] = fpsin(ThetaX<<4)>>4;
    // matRotZ.m[1][0] = fpsin(-ThetaX<<4)>>4;
    // matRotZ.m[1][1] = fpcos(ThetaX<<4)>>4;
    // matRotZ.m[2][2] = 256;
    // matRotZ.m[3][3] = 256;

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
        triangle_fixed triCalc, triRotatedZ, triRotatedZX, triTranslated;

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

        triTranslated = triRotatedZX;
        triTranslated.p[0].z = triRotatedZX.p[0].z + to_fixed(3.0f);
        triTranslated.p[1].z = triRotatedZX.p[1].z + to_fixed(3.0f);
        triTranslated.p[2].z = triRotatedZX.p[2].z + to_fixed(3.0f);
        //  //Rotate in Y-Axis
        // MultiplyMatVec(&triRotatedZX.p[0], &triRotatedZXY.p[0], &matRotY);
        // MultiplyMatVec(&triRotatedZX.p[1], &triRotatedZXY.p[1], &matRotY);
        // MultiplyMatVec(&triRotatedZX.p[2], &triRotatedZXY.p[2], &matRotY);
        vec3d_fixed normal, line1, line2;
        // Calculate normal
        line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
        line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
        line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

        line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
        line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
        line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

        normal.x = fixed_mult(line1.y, line2.z) - fixed_mult(line1.z, line2.y);
        normal.y = fixed_mult(line1.z, line2.x) - fixed_mult(line1.x, line2.z);
        normal.z = fixed_mult(line1.x, line2.y) - fixed_mult(line1.y, line2.x);

        // Calculate length of normal
        fixed length = to_fixed(sqrtf(fixed_to_float(fixed_mult(normal.x, normal.x) + fixed_mult(normal.y, normal.y) + fixed_mult(normal.z, normal.z))));
        
        normal.x = fixed_div(normal.x, length);
        normal.y = fixed_div(normal.y, length);
        normal.z = fixed_div(normal.z, length);
        Serial.println(fixed_to_float(normal.z));
        //if(fixed_to_float(normal.z) > 0.0f)
        if (fixed_to_float((fixed_mult(normal.x, (triTranslated.p[0].x - CameraPos.x)) + fixed_mult(normal.y, (triTranslated.p[0].y - CameraPos.y)) + fixed_mult(normal.z, (triTranslated.p[0].z - CameraPos.z)))) < 0.0f)
        {
            MultiplyMatVecProj(&triRotatedZX.p[0], &triCalc.p[0], proj);
            MultiplyMatVecProj(&triRotatedZX.p[1], &triCalc.p[1], proj);
            MultiplyMatVecProj(&triRotatedZX.p[2], &triCalc.p[2], proj);
            for (byte i = 0; i < 3; i++) // need to scale because the proj matrix gives results between -1 and 1
            {
                triCalc.p[i].x += 256;
                triCalc.p[i].y += 256;
                triCalc.p[i].x = fixed_mult(triCalc.p[i].x, 10752);
                triCalc.p[i].y = fixed_mult(triCalc.p[i].y, 6144);
            }
            // check that no points repeat

            drawTriangle(fixed_to_int(triCalc.p[0].x), fixed_to_int(triCalc.p[0].y), fixed_to_int(triCalc.p[1].x), fixed_to_int(triCalc.p[1].y), fixed_to_int(triCalc.p[2].x), fixed_to_int(triCalc.p[2].y));
        }
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
            (struct vec3d_fixed){256, -256, 256},
            (struct vec3d_fixed){-256, -256, 256}},
            
        (struct triangle_fixed){
            (struct vec3d_fixed){-256, 256, 256},
            (struct vec3d_fixed){256, 256, 256},
            (struct vec3d_fixed){256, -256, 256}},
            

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
// mesh_fixed MeshCubeFixed =
//     {
//         // SOUTH
//         (struct triangle_fixed){
//             (struct vec3d_fixed){0, 0, 0},
//             (struct vec3d_fixed){0, 256, 0},
//             (struct vec3d_fixed){256, 256, 0}},
//         (struct triangle_fixed){
//             (struct vec3d_fixed){0, 0, 0},
//             (struct vec3d_fixed){256, 256, 0},
//             (struct vec3d_fixed){256, 0, 0}},

//         // EAST
//         (struct triangle_fixed){
//             (struct vec3d_fixed){256, 0, -0},
//             (struct vec3d_fixed){256, 256, 0},
//             (struct vec3d_fixed){256, 256, 256}},
//         (struct triangle_fixed){
//             (struct vec3d_fixed){256, 0, 0},
//             (struct vec3d_fixed){256, 256, 256},
//             (struct vec3d_fixed){256, 0, 256}},

//         // NORTH
//         (struct triangle_fixed){
//             (struct vec3d_fixed){256, 0, 256},
//             (struct vec3d_fixed){256, 256, 256},
//             (struct vec3d_fixed){0, 256, 0}},
//         (struct triangle_fixed){
//             (struct vec3d_fixed){256, 0, 256},
//             (struct vec3d_fixed){0, 256, 256},
//             (struct vec3d_fixed){0, 256, 256}},

//         // WEST
//         (struct triangle_fixed){
//             (struct vec3d_fixed){0, 0, 256},
//             (struct vec3d_fixed){0, 256, 256},
//             (struct vec3d_fixed){0, 256, 0}},
//         (struct triangle_fixed){
//             (struct vec3d_fixed){0, 0, 256},
//             (struct vec3d_fixed){0, 256, 0},
//             (struct vec3d_fixed){0, 0, 0}},

//         // TOP
//         (struct triangle_fixed){
//             (struct vec3d_fixed){0, 256, 0},
//             (struct vec3d_fixed){0, 256, 256},
//             (struct vec3d_fixed){256, 256, 256}},
//         (struct triangle_fixed){
//             (struct vec3d_fixed){0, 256, 0},
//             (struct vec3d_fixed){256, 256, 256},
//             (struct vec3d_fixed){256, 256, 0}},

//         // BOTTOM
//         (struct triangle_fixed){
//             (struct vec3d_fixed){256, 0, 256},
//             (struct vec3d_fixed){0, 0, 256},
//             (struct vec3d_fixed){0, 0, 0}},
//         (struct triangle_fixed){
//             (struct vec3d_fixed){256, 0, 256},
//             (struct vec3d_fixed){0, 0, 0},
//             (struct vec3d_fixed){256, 0, 0}},

// };
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