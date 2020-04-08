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
#define SET_UNIFORM_LOCATION(name) p.name = getUniformLocation(p.id, #name);
#define SET_ATTRIB_LOCATION(name) p.name = getAttribLocation(p.id, #name);
#define SET_UNIFORM_LOCATION_ARRAY(name, property) \
    snprintf(str, bufferSize, #name "[%d]." #property, i);\
    p.name[i].property = getUniformLocation(p.id, str);\


CubeProgram
cubeProgramCreate() {
    CubeProgram p;
    SDL_Log("Creating cube program.\n");
    p.id = programCreate("cube.vs", "cube.fs");
    glUseProgram(p.id);

    SET_ATTRIB_LOCATION(aPos);
    SET_ATTRIB_LOCATION(aNormal);
    SET_ATTRIB_LOCATION(aTexCoords);

    SET_UNIFORM_LOCATION(model);
    SET_UNIFORM_LOCATION(modelInverse);
    SET_UNIFORM_LOCATION(projection);
    SET_UNIFORM_LOCATION(view);
    SET_UNIFORM_LOCATION(cameraPosition);

    // longest string could be "light[100].quadratic"
    size_t bufferSize = 25;
    char str[bufferSize];
    for (int i = 0 ; i < LIGHTS_N ; i++) {
        SET_UNIFORM_LOCATION_ARRAY(pointLights, position);
        SET_UNIFORM_LOCATION_ARRAY(pointLights, ambient);
        SET_UNIFORM_LOCATION_ARRAY(pointLights, diffuse);
        SET_UNIFORM_LOCATION_ARRAY(pointLights, specular);
        SET_UNIFORM_LOCATION_ARRAY(pointLights, constant);
        SET_UNIFORM_LOCATION_ARRAY(pointLights, linear);
        SET_UNIFORM_LOCATION_ARRAY(pointLights, quadratic);
    }

    SET_UNIFORM_LOCATION(material.diffuse);
    SET_UNIFORM_LOCATION(material.specular);
    SET_UNIFORM_LOCATION(material.shininess);

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

    SET_ATTRIB_LOCATION(aPos);

    SET_UNIFORM_LOCATION(model);
    SET_UNIFORM_LOCATION(projection);
    SET_UNIFORM_LOCATION(view);

    glUseProgram(0);
    SDL_Log("Light program created.\n");
    return p;
}

