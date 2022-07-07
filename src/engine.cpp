#include <engine.h>
#include <math.h>
#include <driver.h>
void MultiplyMatVec(vec3d *vec_in, vec3d *vec_out, mat4 *m)
{
    vec_out->x = vec_in->x * m->m[0][0] + vec_in->y * m->m[1][0] + vec_in->z * m->m[2][0] + m->m[3][0];
    vec_out->y = vec_in->x * m->m[0][1] + vec_in->y * m->m[1][1] + vec_in->z * m->m[2][1] + m->m[3][1];
    vec_out->z = vec_in->x * m->m[0][2] + vec_in->y * m->m[1][2] + vec_in->z * m->m[2][2] + m->m[3][2];
    float w = vec_in->z * m->m[2][3] + m->m[3][3]; // vec_in->x * m->m[0][3] + vec_in->y * m->m[1][3] +
    if (!w)
    {
        vec_out->x /= w;
        vec_out->y /= w;
        vec_out->z /= w;
    }
}
void MultiplyMatVecProj(vec3d *vec_in, vec3d *vec_out, mat4 *m)
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
float fThetaX = 0;
float fThetaY = 0;
float fFov = 120.0f;
float fFovRad = 1.0f / tanf(fFov * 0.5 / 180.0f * PI);
void draw_cube(mat4 *proj, MouseData *mice)
{
    mat4 matRotZ, matRotX;
    if (((*mice).status & 0x0F) == 0x09)
    {
        fThetaX -= 1.0f * ((float)(*mice).position.x / 10.0f);
        fThetaY += 1.0f * ((float)(*mice).position.y / 10.0f);
    }
    if ((*mice).wheel != 0)
    {
        fFov += (float)mice->wheel * 2.0f;
        fFovRad = 1.0f / tanf(fFov * 0.5 / 180.0f * PI);
        (*proj).m[0][0] = faspect_r * fFovRad;
        (*proj).m[1][1] = fFovRad;
    }
    // println the fthetax in degrees to the serial port
    Serial.print("fthetax: ");
    Serial.println((int)(fThetaX * 180.0f / PI) & 1000);

    // Rotation Z
    matRotZ.m[0][0] = cosf(fThetaX);
    matRotZ.m[0][1] = sinf(fThetaX);
    matRotZ.m[1][0] = -sinf(fThetaX);
    matRotZ.m[1][1] = cosf(fThetaX);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    // Rotation Y
    // matRotY.m[0][0] = cosf(fThetaX);
    // matRotY.m[0][2] = sinf(fThetaX);
    // matRotY.m[1][1] = 1;
    // matRotY.m[2][0] = -sinf(fThetaX);
    // matRotY.m[2][2] = cosf(fThetaX);
    // matRotY.m[3][3] = 1;

    // Rotation X
    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cosf(fThetaY * 0.5f);
    matRotX.m[1][2] = sinf(fThetaY * 0.5f);
    matRotX.m[2][1] = -sinf(fThetaY * 0.5f);
    matRotX.m[2][2] = cosf(fThetaY * 0.5f);
    matRotX.m[3][3] = 1;

    for (auto tri : MeshCube.tris)
    {
        triangle triCalc, triRotatedZ, triRotatedZX;

        // Rotate in Z-Axis
        MultiplyMatVec(&tri.p[0], &triRotatedZ.p[0], &matRotZ);
        MultiplyMatVec(&tri.p[1], &triRotatedZ.p[1], &matRotZ);
        MultiplyMatVec(&tri.p[2], &triRotatedZ.p[2], &matRotZ);

        // Rotate in X-Axis
        MultiplyMatVec(&triRotatedZ.p[0], &triRotatedZX.p[0], &matRotX);
        MultiplyMatVec(&triRotatedZ.p[1], &triRotatedZX.p[1], &matRotX);
        MultiplyMatVec(&triRotatedZ.p[2], &triRotatedZX.p[2], &matRotX);

        //  //Rotate in X-Axis
        // MultiplyMatVec(&triRotatedZX.p[0], &triRotatedZXY.p[0], &matRotY);
        // MultiplyMatVec(&triRotatedZX.p[1], &triRotatedZXY.p[1], &matRotY);
        // MultiplyMatVec(&triRotatedZX.p[2], &triRotatedZXY.p[2], &matRotY);

        MultiplyMatVecProj(&triRotatedZX.p[0], &triCalc.p[0], proj);
        MultiplyMatVecProj(&triRotatedZX.p[1], &triCalc.p[1], proj);
        MultiplyMatVecProj(&triRotatedZX.p[2], &triCalc.p[2], proj);

        // scale the view
        for (byte i = 0; i < 3; i++)
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
            (struct vec3d){-1.0f, 1.0f, -1.0f},
            (struct vec3d){-1.0f, 1.0f, 1.0f},
            (struct vec3d){1.0f, 1.0f, 1.0f}},
        (struct triangle){
            (struct vec3d){-1.0f, 1.0f, -1.0f},
            (struct vec3d){1.0f, 1.0f, 1.0f},
            (struct vec3d){1.0f, 1.0f, -1.0f}},

        // EAST
        (struct triangle){
            (struct vec3d){1.0f, 1.0f, -1.0f},
            (struct vec3d){1.0f, 1.0f, 1.0f},
            (struct vec3d){1.0f, -1.0f, 1.0f}},
        (struct triangle){
            (struct vec3d){1.0f, 1.0f, -1.0f},
            (struct vec3d){1.0f, -1.0f, 1.0f},
            (struct vec3d){1.0f, -1.0f, -1.0f}},

        // NORTH
        (struct triangle){
            (struct vec3d){1.0f, -1.0f, -1.0f},
            (struct vec3d){1.0f, -1.0f, 1.0f},
            (struct vec3d){-1.0f, -1.0f, 1.0f}},
        (struct triangle){
            (struct vec3d){1.0f, -1.0f, -1.0f},
            (struct vec3d){-1.0f, -1.0f, 1.0f},
            (struct vec3d){-1.0f, -1.0f, -1.0f}},

        // WEST
        (struct triangle){
            (struct vec3d){-1.0f, -1.0f, -1.0f},
            (struct vec3d){-1.0f, -1.0f, 1.0f},
            (struct vec3d){-1.0f, 1.0f, 1.0f}},
        (struct triangle){
            (struct vec3d){-1.0f, -1.0f, -1.0f},
            (struct vec3d){-1.0f, 1.0f, 1.0f},
            (struct vec3d){-1.0f, 1.0f, -1.0f}},

        // TOP
        (struct triangle){
            (struct vec3d){-1.0f, 1.0f, 1.0f},
            (struct vec3d){-1.0f, -1.0f, 1.0f},
            (struct vec3d){1.0f, -1.0f, 1.0f}},
        (struct triangle){
            (struct vec3d){-1.0f, 1.0f, 1.0f},
            (struct vec3d){1.0f, -1.0f, 1.0f},
            (struct vec3d){1.0f, 1.0f, 1.0f}},

        // BOTTOM
        (struct triangle){
            (struct vec3d){-1.0f, 1.0f, -1.0f},
            (struct vec3d){-1.0f, -1.0f, -1.0f},
            (struct vec3d){1.0f, -1.0f, -1.0f}},
        (struct triangle){
            (struct vec3d){-1.0f, 1.0f, -1.0f},
            (struct vec3d){1.0f, -1.0f, -1.0f},
            (struct vec3d){1.0f, 1.0f, -1.0f}},

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
    for (byte z = 0; z < 12; z++)
        for (int i = 0; i < 3; i++)
        {
            MeshCube.tris[z].p[i].z += 0.01f;
        }
    return proj;
}
