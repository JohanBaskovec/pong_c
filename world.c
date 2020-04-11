#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include "world.h"
#include <stdbool.h>
#include "maths.h"
#include <math.h>
#include "error.h"


World world;

float defaultBallSpeed = 0.15;
Vec3f defaultBallPosition = {-0, -0, -3};

typedef enum HorizontalBallDirection {
    BALL_HORIZONTAL_DIRECTION_RANDOM,
    BALL_HORIZONTAL_DIRECTION_RIGHT,
    BALL_HORIZONTAL_DIRECTION_LEFT,
} HorizontalBallDirection;

void
ballInit(
        HorizontalBallDirection horizontalDirection
        , bool randomVerticalDirection
        , float verticalDirection
) {
    world.ball = ballCreate();

    Ball *ball = &world.ball;
    ball->position = defaultBallPosition;
    ball->velocity = vec3fZero;
    if (horizontalDirection == BALL_HORIZONTAL_DIRECTION_RANDOM) {
        int d = rand() % 2;
        if (d == 0) {
            horizontalDirection = BALL_HORIZONTAL_DIRECTION_RIGHT;
        } else {
            horizontalDirection = BALL_HORIZONTAL_DIRECTION_LEFT;
        }
    }
    switch (horizontalDirection) {
        case BALL_HORIZONTAL_DIRECTION_LEFT:
            ball->velocity.x = -defaultBallSpeed;
            break;
        case BALL_HORIZONTAL_DIRECTION_RIGHT:
            ball->velocity.x = defaultBallSpeed;
            break;
        default:
            fatalError("Error, invalid ball horizontal direction");
    }
    if (randomVerticalDirection) {
        ball->velocity.y = ((rand() % 1000) / 1000.0f) * defaultBallSpeed;
        bool negative = rand() % 2;
        if (negative) {
            ball->velocity.y = -ball->velocity.y;
        }
    } else {
        ball->velocity.y = verticalDirection * defaultBallSpeed;
    }
    ball->velocity = vec3fMulf(vec3fNormalize(ball->velocity), defaultBallSpeed);
}

void
worldInit() {
    {
        Vec3f paddlePosition = {-5.0, 0.0, -3.0};
        world.paddles[0] = paddleCreate();
        world.paddles[0].position = paddlePosition;
    }

    {
        Vec3f paddlePosition = {5.0, 0.0, -3.0};
        world.paddles[1] = paddleCreate();
        world.paddles[1].position = paddlePosition;
    }

    // floor
    {
        Vec3f pos = {0,-5,0};
        Vec3f size = {14, 1, 20};
        world.walls[0] = wallCreate(pos, size);
    }
    // ceiling
    {
        Vec3f pos = {0,5,0};
        Vec3f size = {14, 1, 20};
        world.walls[1] = wallCreate(pos, size);
    }
    // right wall
    {
        Vec3f pos = {7,0,0};
        Vec3f size = {1, 12, 20};
        Wall wall = wallCreate(pos, size);
        wall.playerIndex = 1;
        world.walls[2] = wall;
    }
    // left wall
    {
        Vec3f pos = {-7,0,0};
        Vec3f size = {1, 12, 20};
        Wall wall = wallCreate(pos, size);
        wall.playerIndex = 0;
        world.walls[3] = wall;
    }
    // back wall
    {
        Vec3f pos = {0,0,-8};
        Vec3f size = {14, 12, 1};
        world.walls[4] = wallCreate(pos, size);
    }

    ballInit(BALL_HORIZONTAL_DIRECTION_RANDOM, false, 0);
}

bool checkBoxesIntersest(
        Vec3f aPos
        , Vec3f aSize
        , Vec3f bPos
        , Vec3f bSize
) {
    float aMinX = aPos.x - aSize.x / 2.0;
    float aMinY = aPos.y - aSize.y / 2.0;
    float aMinZ = aPos.z - aSize.z / 2.0;

    float aMaxX = aPos.x + aSize.x / 2.0;
    float aMaxY = aPos.y + aSize.y / 2.0;
    float aMaxZ = aPos.z + aSize.z / 2.0;

    float bMinX = bPos.x - bSize.x / 2.0;
    float bMinY = bPos.y - bSize.y / 2.0;
    float bMinZ = bPos.z - bSize.z / 2.0;

    float bMaxX = bPos.x + bSize.x / 2.0;
    float bMaxY = bPos.y + bSize.y / 2.0;
    float bMaxZ = bPos.z + bSize.z / 2.0;


  return (aMinX <= bMaxX && aMaxX >= bMinX) &&
         (aMinY <= bMaxY && aMaxY >= bMinY) &&
         (aMinZ <= bMaxZ && aMaxZ >= bMinZ);
}

void ballUpdate() {
    Ball *ball = &world.ball;
    if (ball->moving) {
        Vec3f tempPosition = vec3fAdd(ball->position, ball->velocity);;
        bool collision = false;

        for (int i = 0 ; i < PADDLES_N ; i++) {
            Paddle *paddle = &world.paddles[i];
            bool collision = checkBoxesIntersest(
                    tempPosition
                    , ball->size
                    , paddle->position
                    , paddle->size
            );
            if (collision) {
                collision = true;
                ball->velocity.x *= -1;
                ball->shooter = i;

                float paddleTop = paddle->position.y + paddle->size.y / 2;
                float paddleBottom = paddle->position.y - paddle->size.y / 2;
                float collisionMaxY = paddleTop + ball->size.y / 2.0;
                float collisionMinY = paddleBottom - ball->size.y / 2.0;

                float yDistance = tempPosition.y - collisionMinY;
                float yDistancePercent = yDistance / (collisionMaxY - collisionMinY);
                // mx + b
                float m = -2/0.5;
                float b = 2;
                float yVelocity;
                float mbx;
                if (yDistancePercent > 0.5) {
                    yDistancePercent -= 0.5;
                    mbx = (m * yDistancePercent + b);
                    yVelocity = defaultBallSpeed * mbx;
                } else if (yDistancePercent < 0.5) {
                    mbx = (m * yDistancePercent + b);
                    yVelocity = -defaultBallSpeed * mbx;
                } else {
                    yVelocity = 0;
                }
                ball->velocity.y = yVelocity;
                ball->velocity = vec3fMulf(vec3fNormalize(ball->velocity), defaultBallSpeed);
                break;
            }
        }

        for (int i = 0 ; i < WALLS_N ; i++) {
            Wall *wall = &world.walls[i];
            bool collision = checkBoxesIntersest(
                    tempPosition
                    , ball->size
                    , wall->position
                    , wall->size
            );
            if (collision) {
                collision = true;
                if (wall->playerIndex != -1) {
                    if (ball->shooter != -1) {
                        int playerIndex = wall->playerIndex;
                        Paddle *paddle = &world.paddles[playerIndex];
                        paddle->score++;
                        SDL_Log("Player %d scores against player %d.\nScores: %d:%d\n"
                                , ball->shooter
                                , playerIndex
                                , world.paddles[0].score
                                , world.paddles[1].score
                        );
                    }
                    ballInit(BALL_HORIZONTAL_DIRECTION_RANDOM, true, 0);
                    return;
                }

                ball->velocity.y = -ball->velocity.y;
            }
        }

        tempPosition = vec3fAdd(ball->position, ball->velocity);;
        ball->position = tempPosition;
    }
}

void
worldUpdate() {
    ballUpdate();
}
