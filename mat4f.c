#include "mat4f.h"
#include <math.h>

Mat4f
mat4fIdentity() {
    Mat4f ret;
    ret.m00 = 1;
    ret.m01 = 0;
    ret.m02 = 0;
    ret.m03 = 0;

    ret.m10 = 0;
    ret.m11 = 1;
    ret.m12 = 0;
    ret.m13 = 0;

    ret.m20 = 0;
    ret.m21 = 0;
    ret.m22 = 1;
    ret.m23 = 0;

    ret.m30 = 0;
    ret.m31 = 0;
    ret.m32 = 0;
    ret.m33 = 1;
    return ret;
}

Mat4f
mat4fVec3fTranslate(Mat4f mat, Vec3f vec) {
    Mat4f ret = mat;
    ret.m30 += vec.x;
    ret.m31 += vec.y;
    ret.m32 += vec.z;
    return ret;
}

Mat4f
mat4fPerspective(
        float fovy
        , float aspect
        , float zNear
        , float zFar
) {
    float tanHalfFovy = tanf(fovy / 2.0);
    float zDiff = zFar - zNear;

    Mat4f ret;
    ret.m00 = 1.0 / (aspect * tanHalfFovy);
    ret.m01 = 0;
    ret.m02 = 0;
    ret.m03 = 0;

    ret.m10 = 0;
    ret.m11 = 1.0 / tanHalfFovy;
    ret.m12 = 0;
    ret.m13 = 0;

    ret.m20 = 0;
    ret.m21 = 0;
    ret.m22 = -(zFar + zNear) / zDiff;
    ret.m23 = -1.0;

    ret.m30 = 0;
    ret.m31 = 0;
    ret.m32 = -(2.0 * zFar * zNear) / zDiff;
    ret.m33 = 0;
    return ret;
}

// source: https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
Mat4f
mat4fVec3fRotate(Mat4f mat, float angle, Vec3f vec) {
    float s = sin(angle);
    float c = cos(angle);
    float oneMinC = (1 - c);

    Vec3f axis = vec3fNormalize(vec);

    Mat4f ret;
    ret.m00 = c + (axis.x * axis.x) * oneMinC;
    ret.m01 = axis.y * axis.x * oneMinC + axis.z * s;
    ret.m02 = axis.z * axis.x * oneMinC - axis.y * s;
    ret.m03 = 0;

    ret.m10 = axis.x * axis.y * oneMinC - axis.z * s;
    ret.m11 = c + (axis.y * axis.y) * oneMinC;
    ret.m12 = axis.z * axis.y * oneMinC + axis.x * s;
    ret.m13 = 0;

    ret.m20 = axis.x * axis.z * oneMinC + axis.y * s;
    ret.m21 = axis.y * axis.z * oneMinC - axis.x * s;
    ret.m22 = c + (axis.z * axis.z) * oneMinC;
    ret.m23 = 0;

    ret.m30 = 0;
    ret.m31 = 0;
    ret.m32 = 0;
    ret.m33 = 1;
    return mat4fMulMat4f(mat, ret);
}

Mat4f
mat4fLookAt(Vec3f position, Vec3f target, Vec3f up) {
    Vec3f f = vec3fNormalize(vec3fSub(target, position));

    Vec3f s = vec3fNormalize(vec3fCrossProduct(f, up));

    Vec3f u = vec3fCrossProduct(s, f);

    float sd = vec3fDotProduct(s, position);
    float ud = vec3fDotProduct(u, position);
    float fd = vec3fDotProduct(f, position);

    Mat4f ret;
    ret.m00 = s.x;
    ret.m01 = u.x;
    ret.m02 = -f.x;
    ret.m03 = 0;

    ret.m10 = s.y;
    ret.m11 = u.y;
    ret.m12 = -f.y;
    ret.m13 = 0;

    ret.m20 = s.z;
    ret.m21 = u.z;
    ret.m22 = -f.z;
    ret.m23 = 0;

    ret.m30 = -sd;
    ret.m31 = -ud;
    ret.m32 = fd;
    ret.m33 = 1;
    return ret;
}

Mat4f
mat4fScale(Mat4f mat, Vec3f vec) {
    Mat4f ret = mat;
    ret.m00 *= vec.x;
    ret.m01 *= vec.x;
    ret.m02 *= vec.x;

    ret.m10 *= vec.y;
    ret.m11 *= vec.y;
    ret.m12 *= vec.y;

    ret.m20 *= vec.z;
    ret.m21 *= vec.z;
    ret.m22 *= vec.z;

    return ret;
}

Mat4f
mat4fMulMat4f(Mat4f a, Mat4f b) {
    Mat4f ret;
    ret.m00 = a.m00*b.m00 + a.m10*b.m01 + a.m20*b.m02 + a.m30*b.m03;
    ret.m01 = a.m00*b.m10 + a.m10*b.m11 + a.m20*b.m12 + a.m30*b.m13;
    ret.m02 = a.m00*b.m20 + a.m10*b.m21 + a.m20*b.m22 + a.m30*b.m23;
    ret.m03 = a.m00*b.m30 + a.m10*b.m31 + a.m20*b.m32 + a.m30*b.m33;

    ret.m10 = a.m01*b.m00 + a.m11*b.m01 + a.m21*b.m02 + a.m31*b.m03;
    ret.m11 = a.m01*b.m10 + a.m11*b.m11 + a.m21*b.m12 + a.m31*b.m13;
    ret.m12 = a.m01*b.m20 + a.m11*b.m21 + a.m21*b.m22 + a.m31*b.m23;
    ret.m13 = a.m01*b.m30 + a.m11*b.m31 + a.m21*b.m32 + a.m31*b.m33;

    ret.m20 = a.m02*b.m00 + a.m12*b.m01 + a.m22*b.m02 + a.m32*b.m03;
    ret.m21 = a.m02*b.m10 + a.m12*b.m11 + a.m22*b.m12 + a.m32*b.m13;
    ret.m22 = a.m02*b.m20 + a.m12*b.m21 + a.m22*b.m22 + a.m32*b.m23;
    ret.m23 = a.m02*b.m30 + a.m12*b.m31 + a.m22*b.m32 + a.m32*b.m33;

    ret.m30 = a.m03*b.m00 + a.m13*b.m01 + a.m23*b.m02 + a.m33*b.m03;
    ret.m31 = a.m03*b.m10 + a.m13*b.m11 + a.m23*b.m12 + a.m33*b.m13;
    ret.m32 = a.m03*b.m20 + a.m13*b.m21 + a.m23*b.m22 + a.m33*b.m23;
    ret.m33 = a.m03*b.m30 + a.m13*b.m31 + a.m23*b.m32 + a.m33*b.m33;
    return ret;
}

Mat4f
mat4fMuli(Mat4f mat, int s) {
    Mat4f ret = mat;
    ret.m00 *= s;
    ret.m01 *= s;
    ret.m02 *= s;
    ret.m03 *= s;

    ret.m10 *= s;
    ret.m11 *= s;
    ret.m12 *= s;
    ret.m13 *= s;

    ret.m20 *= s;
    ret.m21 *= s;
    ret.m22 *= s;
    ret.m23 *= s;

    ret.m30 *= s;
    ret.m31 *= s;
    ret.m32 *= s;
    ret.m33 *= s;
    return ret;
}
