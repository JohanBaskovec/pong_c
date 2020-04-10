#ifndef PONG_WORLD
#define PONG_WORLD
#include <stdbool.h>

#include "vec3f.h"
#include "defines.h"
#include "ball.h"
#include "paddle.h"
#include "wall.h"

typedef struct World {
    Paddle paddles[PADDLES_N];

    Ball ball;

    Wall walls[WALLS_N];

    bool ballMoving;

    bool paused;
} World;

extern World world;

void
worldInit();

void
worldUpdate();
#endif
