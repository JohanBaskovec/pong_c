#include "paddle.h"

Paddle
paddleCreate() {
    Paddle paddle;
    paddle.size.x = 0.2;
    paddle.size.y = 2.0;
    paddle.size.z = 2.0;
    paddle.score = 0;
    return paddle;
}
