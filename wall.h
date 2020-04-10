#ifndef PONG_WALL
#define PONG_WALL
#include "vec3f.h"
typedef struct Wall {
    Vec3f position;
    Vec3f size;
    int playerIndex;
} Wall;

Wall
wallCreate(Vec3f position, Vec3f size);
#endif
