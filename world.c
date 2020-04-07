#include "world.h"

Paddle paddle1;
Paddle paddle2;
Vec3f lightPos;

void
worldInit() {
    paddle1.position.x = 0.0;

    lightPos.x = 1.2;
    lightPos.y = 1.0;
    lightPos.z = 2.0;
}
