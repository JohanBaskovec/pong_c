#include "ball.h"

Ball
ballCreate() {
    Ball ball;

    float size = 0.15;
    ball.size.x = size;
    ball.size.y = size;
    ball.size.z = size;

    ball.velocity = vec3fZero;

    ball.moving = true;

    ball.shooter = -1;

    return ball;
}

