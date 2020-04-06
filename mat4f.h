#ifndef PONG_MAT4F
#define PONG_MAT4F
#include "vec3f.h"

typedef struct Mat4f {
    // column 0
    float m00;
    float m01;
    float m02;
    float m03;

    // column 1
    float m10;
    float m11;
    float m12;
    float m13;

    // column 2
    float m20;
    float m21;
    float m22;
    float m23;

    // column 3
    float m30;
    float m31;
    float m32;
    float m33;
} Mat4f;

#define Mat4f_size (16 * sizeof(float))

void
mat4fIdentity(Mat4f *mat);

void
mat4fVec3fTranslate(Mat4f *mat, Vec3f const *vec);

void
mat4fPerspective(
        Mat4f *mat
        , float fovy
        , float aspect
        , float zNear
        , float zFar
);

void
mat4fVec3fRotate(Mat4f *mat, float angle, Vec3f const *vec);

void
mat4fLookAt(
        Mat4f *mat
        , Vec3f const *position
        , Vec3f const *target
        , Vec3f const *up
);
#endif
