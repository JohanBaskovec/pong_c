#ifndef PONG_GRAPHICS
#define PONG_GRAPHICS
#include <SDL.h>
#include <GL/glew.h>

#define ZNEAR (0.1)
#define ZFAR (100.0)

void
graphicsInit();

void
graphicsRender();

void
graphicsFree();
#endif
