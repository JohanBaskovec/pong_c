#ifndef PONG_INPUT
#define PONG_INPUT
#include <stdbool.h>

typedef struct PressedKeys {
    bool escape;
    bool up;
    bool down;
    bool cameraUp;
    bool cameraDown;
    bool cameraLeft;
    bool cameraRight;
    bool cameraFront;
    bool cameraBack;
} PressedKeys;

extern PressedKeys pressedKeys;

void
keyDown(SDL_KeyboardEvent *e);

void
keyUp(SDL_KeyboardEvent *e);

void
mouseMove(SDL_MouseMotionEvent e);

void
inputPollEvents();

#endif
