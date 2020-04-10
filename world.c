#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include "world.h"
#include <stdbool.h>
#include "maths.h"
#include <math.h>


World world;

float defaultBallSpeed = 0.2;
Vec3f defaultBallPosition = {0, 0, -3};

typedef enum HorizontalBallDirection {
    BALL_HORIZONTAL_DIRECTION_RANDOM,
    BALL_HORIZONTAL_DIRECTION_RIGHT,
    BALL_HORIZONTAL_DIRECTION_LEFT,
} HorizontalBallDirection;

void
ballInit(HorizontalBallDirection horizontalDirection, bool randomYDirection) {
    world.ball = ballCreate();

    Ball *ball = &world.ball;
    ball->position = defaultBallPosition;
    ball->velocity = vec3fZero;
    if (horizontalDirection == BALL_HORIZONTAL_DIRECTION_RANDOM) {
        int d = rand() % 2;
        if (d == 0) {
            ball->velocity.x = -defaultBallSpeed;
        } else {
            ball->velocity.x = defaultBallSpeed;
        }
    }
    if (randomYDirection) {
        ball->velocity.y = ((rand() % 1000) / 1000.0f) * defaultBallSpeed;
        bool negative = rand() % 2;
        if (negative) {
            ball->velocity.y = -ball->velocity.y;
        }
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

    ballInit(BALL_HORIZONTAL_DIRECTION_RANDOM, true);
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
        ball->position.x += ball->velocity.x;
        ball->position.y += ball->velocity.y;

        for (int i = 0 ; i < PADDLES_N ; i++) {
            Paddle *paddle = &world.paddles[i];
            bool collision = checkBoxesIntersest(
                    ball->position
                    , ball->size
                    , paddle->position
                    , paddle->size
            );
            if (collision) {
                ball->velocity.x = -ball->velocity.x;
                ball->shooter = i;
            }
        }

        for (int i = 0 ; i < WALLS_N ; i++) {
            Wall *wall = &world.walls[i];
            bool collision = checkBoxesIntersest(
                    ball->position
                    , ball->size
                    , wall->position
                    , wall->size
            );
            if (collision) {
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
                    ballInit(BALL_HORIZONTAL_DIRECTION_RANDOM, true);
                    return;
                }

                ball->velocity.y = -ball->velocity.y;
            }
        }
    }
}

void
worldUpdate() {
    ballUpdate();
}
