#ifndef PONG_INPUT
#define PONG_INPUT
#include <stdbool.h>
#include <SDL_scancode.h>

typedef enum Key {
    KEY_NONE
    , KEY_ESCAPE

    , KEY_PADDLE_MOVE_DOWN
    , KEY_PADDLE_MOVE_UP
    , KEY_PADDLE_MOVE_LEFT
    , KEY_PADDLE_MOVE_RIGHT
    , KEY_PADDLE_MOVE_BACK
    , KEY_PADDLE_MOVE_FRONT

    , KEY_CAMERA_MOVE_LEFT
    , KEY_CAMERA_MOVE_RIGHT
    , KEY_CAMERA_MOVE_FRONT
    , KEY_CAMERA_MOVE_BACK

    , KEY_NUMBER
} Key;

extern bool pressedKeys[KEY_NUMBER];
extern Key keyMapping[SDL_NUM_SCANCODES];

void
inputInit();

void
keyDown(SDL_KeyboardEvent *e);

void
keyUp(SDL_KeyboardEvent *e);

void
mouseMove(SDL_MouseMotionEvent e);

void
inputPollEvents();

#endif
