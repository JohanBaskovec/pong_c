#include "maths.h"

void
mat4fIdentity(Mat4f *mat) {
    mat->m00 = 1;
    mat->m01 = 0;
    mat->m02 = 0;
    mat->m03 = 0;

    mat->m10 = 0;
    mat->m11 = 1;
    mat->m12 = 0;
    mat->m13 = 0;

    mat->m20 = 0;
    mat->m21 = 0;
    mat->m22 = 1;
    mat->m23 = 0;

    mat->m30 = 0;
    mat->m31 = 0;
    mat->m32 = 0;
    mat->m33 = 1;
}

void
mat4fVec3fTranslate(Mat4f *mat, Vec3f *vec) {
    mat->m30 += vec->x;
    mat->m31 += vec->y;
    mat->m32 += vec->z;
}

void
mat4fPerspective(
        Mat4f *mat
        , float fovy
        , float aspect
        , float zNear
        , float zFar
) {
    float tanHalfFovy = tanf(fovy / 2.0);
    float zDiff = zFar - zNear;

    mat->m00 = 1.0 / (aspect * tanHalfFovy);
    mat->m01 = 0;
    mat->m02 = 0;
    mat->m03 = 0;

    mat->m10 = 0;
    mat->m11 = 1.0 / tanHalfFovy;
    mat->m12 = 0;
    mat->m13 = 0;

    mat->m20 = 0;
    mat->m21 = 0;
    mat->m22 = -(zFar + zNear) / zDiff;
    mat->m23 = -1.0;

    mat->m30 = 0;
    mat->m31 = 0;
    mat->m32 = -(2.0 * zFar * zNear) / zDiff;
    mat->m33 = 0;
}
