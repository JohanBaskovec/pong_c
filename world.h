#ifndef PONG_WORLD
#define PONG_WORLD
#include "vec3f.h"
#include "defines.h"

typedef struct Paddle {
    Vec3f position;
} Paddle;

extern Paddle paddles[PADDLES_N];

extern Vec3f lightPos[LIGHTS_N];

void
worldInit();

#endif
