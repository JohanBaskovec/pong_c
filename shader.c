#include <SDL.h>
#include <GL/glew.h>
#include <stdbool.h>
#include "error.h"
#include "fs.h"

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
        SDL_Log("Compiled shader %d\n", shader);
        return shader;
    }

    char errorMessage[1024];
    GLsizei errorMessageLength = 0;
    glGetShaderInfoLog(shader, 1024, &errorMessageLength, errorMessage);
    fatalError("glCompileShader error: %s\n", errorMessage);
    return 0;
}

void
readShaderSource(char const *name, char **target) {
    size_t length;
    ErrorCode err = getContentsPosix(name, target , &length);
    printf("HEIN %s %d\n", name, err);
    if (err) {
        fatalError("Error while reading shader source %s, %d %s\n", name,
                err, errorGetString(err));
    }

}
