#ifndef PONG_SHADER
#define PONG_SHADER


GLuint
programCreate(
        char const *vertexShader,
        char const *fragmentShader
);

typedef struct CubeProgram {
    GLuint id;

    GLint aPos;
    GLint aNormal;

    GLint model;
    GLint view;
    GLint projection;

    GLint objectColor;
    GLint lightColor;
    GLint lightPosition;
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
