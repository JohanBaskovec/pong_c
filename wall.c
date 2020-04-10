#include "wall.h"

Wall
wallCreate(Vec3f position, Vec3f size) {
    Wall wall = {
        .position = position,
        .size = size,
        .playerIndex = -1,
    };

    return wall;
}
