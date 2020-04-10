#ifndef PONG_PADDLE
#define PONG_PADDLE
#include "vec3f.h"
typedef struct Paddle {
    Vec3f position;
    Vec3f size;
    int score;
} Paddle;

Paddle
paddleCreate();
#endif
