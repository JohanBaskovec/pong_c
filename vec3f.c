#include <math.h>
#include "vec3f.h"

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
