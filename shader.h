#ifndef PONG_SHADER
#define PONG_SHADER
#include "defines.h"

GLuint
programCreate(
        char const *vertexShader,
        char const *fragmentShader
);

typedef struct MaterialVars {
    GLint diffuse;
    GLint specular;
    GLint shininess;
} MaterialVars;

typedef struct LightVars {
    GLint position;
    GLint ambient;
    GLint diffuse;
    GLint specular;
    GLint constant;
    GLint linear;
    GLint quadratic;
} LightVars;

typedef struct CubeProgram {
    GLuint id;

    GLint aPos;
    GLint aNormal;
    GLint aTexCoords;

    GLint model;
    GLint modelInverse;
    GLint view;
    GLint projection;

    GLint cameraPosition;

    MaterialVars material;
    LightVars pointLights[LIGHTS_N];
} CubeProgram;


CubeProgram
cubeProgramCreate();

typedef struct LightProgram {
    GLuint id;

    GLint aPos;

    GLint model;
    GLint view;
    GLint projection;
} LightProgram;

LightProgram
lightProgramCreate();

#endif
