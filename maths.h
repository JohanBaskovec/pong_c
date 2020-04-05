#ifndef PONG_MATHS_H
#define PONG_MATHS_H

#include <math.h>

#define radiansToDegreesRatio (57.2957795)
#define degreesToRadiansRatio (0.0174532925)

#define radiansToDegrees(x) (x*radiansToDegreesRatio)
#define degreesToRadians(x) (x*degreesToRadiansRatio)

typedef struct Vec3f {
    float x;
    float y;
    float z;
} Vec3f;

#define Vec3f_size (3 * sizeof(float))

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
mat4fMul(Mat4f *a, Mat4f const *b);

void
mat4fVec3fRotate(Mat4f *mat, float angle, Vec3f const *vec);

void
mat4fLookAt(
        Mat4f *mat
        , Vec3f const *position
        , Vec3f const *target
        , Vec3f const *up
);

void
vec3fNormalize(Vec3f *vec);

float
vec3fLength(Vec3f const *vec);

void
vec3fAdd(Vec3f *a, Vec3f const *b);

void
vec3fSub(Vec3f *a, Vec3f const *b);

float
vec3fDotProduct(Vec3f const *a, Vec3f const *b);

void
vec3fCrossProduct(Vec3f *a, Vec3f const *b);


#endif
