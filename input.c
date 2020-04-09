#include <SDL.h>
#include "input.h"
#include "camera.h"

float mouseSensitivity = 0.1f;
PressedKeys pressedKeys;

void
inputPollEvents() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                pressedKeys.escape = true;
                break;
            case SDL_KEYDOWN:
                keyDown(&e.key);
                break;
            case SDL_KEYUP:
                keyUp(&e.key);
                break;
            case SDL_MOUSEMOTION:
                mouseMove(e.motion);
                break;
            default:
                // do nothing
                break;
        }
    }
}


void
mouseMove(SDL_MouseMotionEvent e) {
    float xOffset = e.xrel * mouseSensitivity;
    float yOffset = -e.yrel * mouseSensitivity;
    camera.yaw += xOffset;
    camera.pitch += yOffset;
    updateCameraVector();
}

void
keyDown(SDL_KeyboardEvent *e) {
    switch (e->keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
            pressedKeys.escape = true;
            break;
        case SDL_SCANCODE_S:
            pressedKeys.down = true;
            break;
        case SDL_SCANCODE_W:
            pressedKeys.up = true;
            break;
        case SDL_SCANCODE_UP:
            //pressedKeys.cameraUp = true;
            pressedKeys.cameraFront = true;
            break;
        case SDL_SCANCODE_RIGHT:
            pressedKeys.cameraRight = true;
            break;
        case SDL_SCANCODE_DOWN:
            //pressedKeys.cameraDown = true;
            pressedKeys.cameraBack = true;
            break;
        case SDL_SCANCODE_LEFT:
            pressedKeys.cameraLeft = true;
            break;
        default:
            // do nothing
            break;
    }
}

void
keyUp(SDL_KeyboardEvent *e) {
    switch (e->keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
            pressedKeys.escape = false;
            break;
        case SDL_SCANCODE_S:
            pressedKeys.down = false;
            break;
        case SDL_SCANCODE_W:
            pressedKeys.up = false;
            break;
        case SDL_SCANCODE_UP:
            //pressedKeys.cameraUp = false;
            pressedKeys.cameraFront = false;
            break;
        case SDL_SCANCODE_RIGHT:
            pressedKeys.cameraRight = false;
            break;
        case SDL_SCANCODE_DOWN:
            //pressedKeys.cameraDown = false;
            pressedKeys.cameraBack = false;
            break;
        case SDL_SCANCODE_LEFT:
            pressedKeys.cameraLeft = false;
            break;
        default:
            // do nothing
            break;
    }
}



