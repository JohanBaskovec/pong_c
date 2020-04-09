#include <SDL.h>
#include "input.h"
#include "camera.h"

float mouseSensitivity = 0.1f;
bool pressedKeys[KEY_NUMBER];
Key keyMapping[SDL_NUM_SCANCODES];

void
inputInit() {
    keyMapping[SDL_SCANCODE_ESCAPE] = KEY_ESCAPE;

    keyMapping[SDL_SCANCODE_S] =     KEY_PADDLE_MOVE_DOWN;
    keyMapping[SDL_SCANCODE_W] =     KEY_PADDLE_MOVE_UP;
    keyMapping[SDL_SCANCODE_A] =     KEY_PADDLE_MOVE_LEFT;
    keyMapping[SDL_SCANCODE_D] =     KEY_PADDLE_MOVE_RIGHT;
    keyMapping[SDL_SCANCODE_Q] =     KEY_PADDLE_MOVE_BACK;
    keyMapping[SDL_SCANCODE_E] =     KEY_PADDLE_MOVE_FRONT;

    keyMapping[SDL_SCANCODE_UP] =    KEY_CAMERA_MOVE_FRONT;
    keyMapping[SDL_SCANCODE_RIGHT] = KEY_CAMERA_MOVE_RIGHT;
    keyMapping[SDL_SCANCODE_DOWN] =  KEY_CAMERA_MOVE_BACK;
    keyMapping[SDL_SCANCODE_LEFT] =  KEY_CAMERA_MOVE_LEFT;
}

void
inputPollEvents() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                pressedKeys[KEY_ESCAPE] = true;
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
    Key pressedKey = keyMapping[e->keysym.scancode];
    pressedKeys[pressedKey] = true;
}

void
keyUp(SDL_KeyboardEvent *e) {
    Key releasedKey = keyMapping[e->keysym.scancode];
    pressedKeys[releasedKey] = false;
}

