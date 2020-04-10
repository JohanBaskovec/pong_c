#ifndef PONG_BALL
#define PONG_BALL
#include <stdbool.h>
#include "vec3f.h"
typedef struct Ball {
    Vec3f position;
    Vec3f size;
    Vec3f velocity;
    bool moving;
    int shooter;
} Ball;

Ball
ballCreate();

void
ballUpdate();
#endif
