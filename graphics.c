#include "graphics.h"
#include <stdbool.h>
#include "error.h"
#include "camera.h"
#include "world.h"
#include "mat4f.h"
#include "vec3f.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include "maths.h"

#include "3d_data.h"

static int screenWidth = 800;
static int screenHeight = 600;
static float screenRatio = 0;
static float fovDegree = 90;
static float fovRadians = 0;

static GLchar *vertexShaderSource[] = {""};
static GLchar *fragmentShaderSource[] = {""};

GLuint vao;
GLuint vbo;
GLuint ibo;
SDL_Window *window;
GLuint program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint paddleTexture;

GLint inTextureCoordVar;
GLint vertexVar;
GLint modelVar;
GLint projectionVar;
GLint viewVar;
GLint paddleTextureVar;

void
openglDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                           GLenum severity, GLsizei length,
                           const GLchar* message, const void* userParam) {
    SDL_Log(
            "GL CALLBACK: type = 0x%x, severity = 0x%x, message = %s\n"
            , type
            , severity
            , message
    );
}


void
graphicsInit() {
    readShaderSource("cube.vs", vertexShaderSource);
    readShaderSource("cube.fs", fragmentShaderSource);

    screenRatio = (screenWidth * 1.0) / screenHeight;
    fovRadians = degreesToRadians(fovDegree);
    int err = SDL_Init(SDL_INIT_VIDEO);
    if (err) {
      fatalSdlError("SDL_Init");
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_PROFILE_MASK
            , SDL_GL_CONTEXT_PROFILE_CORE
    );


    window = SDL_CreateWindow(
        "Pong"
        , SDL_WINDOWPOS_CENTERED
        , SDL_WINDOWPOS_CENTERED
        , screenWidth
        , screenHeight
        , SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        fatalSdlError("SDL_CreateWindow");
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (err) {
        fatalSdlError("SDL_GL_CreateContext");
    }

    glewExperimental = true;
    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, screenWidth, screenHeight);

    glDebugMessageCallback(openglDebugMessageCallback, NULL);

    err = SDL_GL_SetSwapInterval(1);

    if (err) {
      fatalSdlError("SDL_GL_SetSwapInterval");
    }

    program = glCreateProgram();

    vertexShader = createShader(
            GL_VERTEX_SHADER
            , vertexShaderSource
            , program
    );

    fragmentShader = createShader(
            GL_FRAGMENT_SHADER
            , fragmentShaderSource
            , program
    );

    glLinkProgram(program);

    glUseProgram(program);

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

    inTextureCoordVar = glGetAttribLocation(program, "inTextureCoord");
    vertexVar = glGetAttribLocation(program, "LVertexPos2D");
    modelVar = glGetUniformLocation(program, "model");
    projectionVar = glGetUniformLocation(program, "projection");
    viewVar = glGetUniformLocation(program, "view");
    paddleTextureVar = glGetUniformLocation(program, "paddleTexture");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // set value of the currently bound buffer of that type
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER
            , sizeof(indices)
            , indices
            , GL_STATIC_DRAW
    );

    glVertexAttribPointer(
            vertexVar
            , 3
            , GL_FLOAT
            , GL_FALSE
            , 5 * sizeof(float)
            , NULL
    );

    glEnableVertexAttribArray(vertexVar);

    glVertexAttribPointer(
            inTextureCoordVar
            , 2
            , GL_FLOAT
            , GL_FALSE
            , 5 * sizeof(float)
            , (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(inTextureCoordVar);

    glGenTextures(1, &paddleTexture);

    glBindTexture(GL_TEXTURE_2D, paddleTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("paddle.png", &width, &height, &nrChannels, 0);
    if (!data) {
        fatalError("stbi_load error.\n");
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glUniform1i(paddleTextureVar, paddleTexture);

    glBindVertexArray(0);

    SDL_Log("OpengL: available image units = %d\n",  GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    SDL_SetRelativeMouseMode(true);

}

void
graphicsRender() {
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    camera.target = vec3fAdd(camera.position, camera.front);
    Mat4f viewMat = mat4fLookAt(camera.position, camera.target, camera.up);
    glUniformMatrix4fv(viewVar, 1, false, (GLfloat*)&viewMat);

    Mat4f projectionMat = mat4fPerspective(fovRadians, screenRatio, ZNEAR, ZFAR);
    glUniformMatrix4fv(projectionVar, 1, false, (GLfloat*)&projectionMat);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, paddleTexture);

    glBindVertexArray(vao);

    Vec3f rotate = {
        .x = 1.0,
        .y = 0.0,
        .z = 0.0
    };
    Mat4f modelMat = mat4fIdentity();
    modelMat = mat4fVec3fRotate(modelMat, SDL_GetTicks() * degreesToRadians(-0.1), rotate);
    modelMat = mat4fVec3fTranslate(modelMat, paddle1.position);
    glUniformMatrix4fv(modelVar, 1, false, (GLfloat*)&modelMat);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glUseProgram(0);
    SDL_GL_SwapWindow(window);
}

void
graphicsFree() {
    SDL_DestroyWindow(window);
}
