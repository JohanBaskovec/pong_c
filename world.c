#include "world.h"

Paddle paddles[PADDLES_N];

Vec3f lightPos[LIGHTS_N];

Vec3f floorPos;

void
worldInit() {
    paddles[0].position.x = 0.0;

    paddles[1].position.z = -1.0;
    paddles[1].position.x = -1.0;

    lightPos[0].x = 1.0;
    lightPos[0].z = 1.0;

    lightPos[1].x = 2.0;
    lightPos[1].y = -1.0;
    lightPos[1].z = -1.0;

    lightPos[2].x = -3.0;
    lightPos[2].z = -2.0;

    floorPos.y = -22.0;
}
