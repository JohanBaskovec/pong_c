#ifndef PONG_VEC3F
#define PONG_VEC3F

typedef struct Vec3f {
    float x;
    float y;
    float z;
} Vec3f;

#define Vec3f_size (3 * sizeof(float))

float
vec3fLength(Vec3f const *vec);

void
vec3fNormalize(Vec3f *vec);

void
vec3fAdd(Vec3f *a, Vec3f const *b);

void
vec3fSub(Vec3f *a, Vec3f const *b);

float
vec3fDotProduct(Vec3f const *a, Vec3f const *b);

void
vec3fCrossProduct(Vec3f *a, Vec3f const *b);

#endif
