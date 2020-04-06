#include <SDL.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdbool.h>
#include "maths.h"
#include "fs.h"
#include "3d_data.h"
#include "shader.h"
#include "camera.h"
#include "graphics.h"
#include "input.h"
#include "world.h"
#include "mat4f.h"

float movementSpeed = 0.1;

int main(int argc, char *argv[]) {
    cameraInit();
    graphicsInit();

    while (true) {
        graphicsRender();

        inputPollEvents();

        if (pressedKeys.escape) {
            goto end;
        }
        if (pressedKeys.up) {
            paddle1.position.y += movementSpeed;
        } else if (pressedKeys.down) {
            paddle1.position.y -= movementSpeed;
        }
        if (pressedKeys.cameraUp) {
            camera.position.y += 0.2;
        } else if (pressedKeys.cameraDown) {
            camera.position.y -= 0.2;
        }
        if (pressedKeys.cameraBack) {
            camera.position.x -= movementSpeed * camera.front.x;
            camera.position.y -= movementSpeed * camera.front.y;
            camera.position.z -= movementSpeed * camera.front.z;
        } else if (pressedKeys.cameraFront) {
            camera.position.x += movementSpeed * camera.front.x;
            camera.position.y += movementSpeed * camera.front.y;
            camera.position.z += movementSpeed * camera.front.z;
        }
        if (pressedKeys.cameraLeft) {
            camera.position.x -= movementSpeed * camera.right.x;
            camera.position.y -= movementSpeed * camera.right.y;
            camera.position.z -= movementSpeed * camera.right.z;
        } else if (pressedKeys.cameraRight) {
            camera.position.x += movementSpeed * camera.right.x;
            camera.position.y += movementSpeed * camera.right.y;
            camera.position.z += movementSpeed * camera.right.z;
        }

    }

end:

    graphicsFree();
    SDL_Quit();

    return 0;
}

