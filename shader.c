#include <SDL.h>
#include <GL/glew.h>
#include <stdbool.h>
#include "error.h"
#include "fs.h"
#include "shader.h"

void
readShaderSource(char const *name, char **target) {
    size_t length;
    ErrorCode err = getContentsPosix(name, target , &length);
    if (err) {
        fatalError("Error while reading shader source %s, %d %s\n", name,
                err, errorGetString(err));
    }

}

GLuint createShader(
        GLenum shaderType
        , GLchar **source
        , GLuint program
) {
    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, (GLchar const* const *)source, NULL);

    glCompileShader(shader);

    GLint compiled = false;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (compiled) {
        glAttachShader(program, shader);
        return shader;
    }

    char errorMessage[1024];
    GLsizei errorMessageLength = 0;
    glGetShaderInfoLog(shader, 1024, &errorMessageLength, errorMessage);
    fatalError("glCompileShader error: %s\n", errorMessage);
    return 0;
}



GLuint
programCreate(
        char const *vertexShaderFileName,
        char const *fragmentShaderFileName
) {
    char *vsSrc;
    char *fsSrc;

    readShaderSource(vertexShaderFileName, &vsSrc);
    readShaderSource(fragmentShaderFileName, &fsSrc);

    GLuint program = glCreateProgram();

    GLuint vertexShader = createShader(
            GL_VERTEX_SHADER
            , &vsSrc
            , program
    );
    SDL_Log("Created vertex shader from %s.", vertexShaderFileName);

    GLuint fragmentShader = createShader(
            GL_FRAGMENT_SHADER
            , &fsSrc
            , program
    );
    SDL_Log("Created fragment shader from %s.", fragmentShaderFileName);

    glLinkProgram(program);

    GLint linkSuccess = false;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess) {
        char errorMessage[1024];
        GLsizei errorMessageLength = 0;
        glGetProgramInfoLog(program, 1024, &errorMessageLength, errorMessage);
        fatalError("glLinkProgram: error: %s\n", errorMessage);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    SDL_Log("Created program %d.\n", program);
    return program;
}

GLint
getUniformLocation(GLuint program, char const *name) {
    GLint var = glGetUniformLocation(program, name);
    if (var == -1) {
        fatalError("Uniform %s not found in shader.\n", name);
    }
    return var;
}

GLint
getAttribLocation(GLuint program, char const *name) {
    GLint var = glGetAttribLocation(program, name);
    if (var == -1) {
        fatalError("Attribute %s not found in shader.\n", name);
    }
    return var;
}
#define TO_STR(a) TO_STR2(a)
#define TO_STR2(a) #a
#define SET_UNIFORM(name) p.name = getUniformLocation(p.id, TO_STR(#name));
#define SET_ATTRIB(name) p.name = getAttribLocation(p.id, TO_STR(#name));

CubeProgram
cubeProgramCreate() {
    CubeProgram p;
    SDL_Log("Creating cube program.\n");
    p.id = programCreate("cube.vs", "cube.fs");
    glUseProgram(p.id);

    p.aPos = getAttribLocation(p.id, "aPos");
    p.aNormal = getAttribLocation(p.id, "aNormal");
    //SET_ATTRIB(aNormal);

    p.model = getUniformLocation(p.id, "model");
    p.projection = getUniformLocation(p.id, "projection");
    p.view = getUniformLocation(p.id, "view");

    p.objectColor = getUniformLocation(p.id, "objectColor");
    p.lightColor = getUniformLocation(p.id, "lightColor");
    p.lightPosition = getUniformLocation(p.id, "lightPosition");
    //SET_UNIFORM(lightPosition);

    glUseProgram(0);
    SDL_Log("Cube program created.\n");
    return p;
}

LightProgram
lightProgramCreate() {
    LightProgram p;
    SDL_Log("Creating cube program.\n");
    p.id = programCreate("light.vs", "light.fs");
    glUseProgram(p.id);

    p.aPos = getAttribLocation(p.id, "aPos");

    p.model = getUniformLocation(p.id, "model");
    p.projection = getUniformLocation(p.id, "projection");
    p.view = getUniformLocation(p.id, "view");

    glUseProgram(0);
    SDL_Log("Light program created.\n");
    return p;
}

