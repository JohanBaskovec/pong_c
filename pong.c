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
    inputInit();
    graphicsInit();
    worldInit();

    while (true) {
        graphicsRender();

        inputPollEvents();

        if (pressedKeys[KEY_ESCAPE]) {
            goto end;
        }
        if (pressedKeys[KEY_PADDLE_MOVE_UP]) {
            paddles[0].position.y += movementSpeed;
        } else if (pressedKeys[KEY_PADDLE_MOVE_DOWN]) {
            paddles[0].position.y -= movementSpeed;
        }
        if (pressedKeys[KEY_PADDLE_MOVE_LEFT]) {
            paddles[0].position.x -= movementSpeed;
        } else if (pressedKeys[KEY_PADDLE_MOVE_RIGHT]) {
            paddles[0].position.x += movementSpeed;
        }
        if (pressedKeys[KEY_PADDLE_MOVE_BACK]) {
            paddles[0].position.z -= movementSpeed;
        } else if (pressedKeys[KEY_PADDLE_MOVE_FRONT]) {
            paddles[0].position.z += movementSpeed;
        }
        if (pressedKeys[KEY_CAMERA_MOVE_BACK]) {
            camera.position.x -= movementSpeed * camera.front.x;
            camera.position.y -= movementSpeed * camera.front.y;
            camera.position.z -= movementSpeed * camera.front.z;
        } else if (pressedKeys[KEY_CAMERA_MOVE_FRONT]) {
            camera.position.x += movementSpeed * camera.front.x;
            camera.position.y += movementSpeed * camera.front.y;
            camera.position.z += movementSpeed * camera.front.z;
        }
        if (pressedKeys[KEY_CAMERA_MOVE_LEFT]) {
            camera.position.x -= movementSpeed * camera.right.x;
            camera.position.y -= movementSpeed * camera.right.y;
            camera.position.z -= movementSpeed * camera.right.z;
        } else if (pressedKeys[KEY_CAMERA_MOVE_RIGHT]) {
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

