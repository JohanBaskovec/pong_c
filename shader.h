#ifndef PONG_SHADER
#define PONG_SHADER

GLuint createShader(
        GLenum shaderType
        , GLchar **source
        , GLuint program
);

void
readShaderSource(char const *name, char **target);

#endif
