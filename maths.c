#include "maths.h"
#include <stdio.h>

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
mat4fVec3fTranslate(Mat4f *mat, Vec3f const *vec) {
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

float
vec3fLength(Vec3f const *vec) {
    return sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
}

void
vec3fNormalize(Vec3f *vec) {
    float length = vec3fLength(vec);
    vec->x = vec->x / length;
    vec->y = vec->y / length;
    vec->z = vec->z / length;
}

// source: https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
void
mat4fVec3fRotate(Mat4f *mat, float angle, Vec3f const *vec) {
    float s = sin(angle);
    float c = cos(angle);
    float oneMinC = (1 - c);

    // copy vector
    Vec3f axis = *vec;
    vec3fNormalize(&axis);

    mat->m00 = c + (axis.x * axis.x) * oneMinC;
    mat->m01 = axis.y * axis.x * oneMinC + axis.z * s;
    mat->m02 = axis.z * axis.x * oneMinC - axis.y * s;
    mat->m03 = 0;

    mat->m10 = axis.x * axis.y * oneMinC - axis.z * s;
    mat->m11 = c + (axis.y * axis.y) * oneMinC;
    mat->m12 = axis.z * axis.y * oneMinC + axis.x * s;
    mat->m13 = 0;

    mat->m20 = axis.x * axis.z * oneMinC + axis.y * s;
    mat->m21 = axis.y * axis.z * oneMinC - axis.x * s;
    mat->m22 = c + (axis.z * axis.z) * oneMinC;
    mat->m23 = 0;

    mat->m30 = 0;
    mat->m31 = 0;
    mat->m32 = 0;
    mat->m33 = 1;
}

void
mat4fLookAt(
        Mat4f *mat
        , Vec3f const *position
        , Vec3f const *target
        , Vec3f const *up
) {
    Vec3f f = *target;
    vec3fSub(&f, position);
    vec3fNormalize(&f);

    Vec3f s = f;
    vec3fCrossProduct(&s, up);
    vec3fNormalize(&s);

    Vec3f u = s;
    vec3fCrossProduct(&u, &f);

    float sd = vec3fDotProduct(&s, position);
    float ud = vec3fDotProduct(&u, position);
    float fd = vec3fDotProduct(&f, position);

    mat->m00 = s.x;
    mat->m01 = u.x;
    mat->m02 = -f.x;
    mat->m03 = 0;

    mat->m10 = s.y;
    mat->m11 = u.y;
    mat->m12 = -f.y;
    mat->m13 = 0;

    mat->m20 = s.z;
    mat->m21 = u.z;
    mat->m22 = -f.z;
    mat->m23 = 0;

    mat->m30 = -sd;
    mat->m31 = -ud;
    mat->m32 = fd;
    mat->m33 = 1;
}

void
vec3fAdd(Vec3f *a, Vec3f const *b) {
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}

void
vec3fSub(Vec3f *a, Vec3f const *b) {
    a->x -= b->x;
    a->y -= b->y;
    a->z -= b->z;
}

float
vec3fDotProduct(Vec3f const *a, Vec3f const *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void
vec3fCrossProduct(Vec3f *a, Vec3f const *b) {
    float x = a->x;
    float y = a->y;
    float z = a->z;
    a->x = y * b->z - z * b->y;
    a->y = z * b->x - x * b->z;
    a->z = x * b->y - y * b->x;
}
