#ifndef PONG_WORLD
#define PONG_WORLD
#include "vec3f.h"

typedef struct Paddle {
    Vec3f position;

} Paddle;

extern Paddle paddle1;
extern Paddle paddle2;
extern Vec3f lightPos;

void
worldInit();

#endif
