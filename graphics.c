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

GLuint cubeVao;
GLuint vbo;
GLuint ibo;
GLuint lightVao;
SDL_Window *window;
GLuint paddleTexture;

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

CubeProgram cubeProgram;
LightProgram lightProgram;

void
graphicsInit() {
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

    cubeProgram = cubeProgramCreate();
    lightProgram = lightProgramCreate();

    glGenVertexArrays(1, &cubeVao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(cubeVao);
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
            cubeProgram.aPos
            , 3
            , GL_FLOAT
            , GL_FALSE
            , 8 * sizeof(float)
            , NULL
    );
    glEnableVertexAttribArray(cubeProgram.aPos);

    glVertexAttribPointer(
            cubeProgram.aNormal
            , 3
            , GL_FLOAT
            , GL_FALSE
            , 8 * sizeof(float)
            , (void*)(5 * sizeof(float))

    );
    glEnableVertexAttribArray(cubeProgram.aNormal);

    /*
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
    */

    glGenVertexArrays(1, &lightVao);
    glBindVertexArray(lightVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(
            lightProgram.aPos
            , 3
            , GL_FLOAT
            , GL_FALSE
            , 8 * sizeof(float)
            , NULL
    );
    glEnableVertexAttribArray(lightProgram.aPos);

    glBindVertexArray(0);

    SDL_Log("OpengL: available image units = %d\n",  GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    SDL_SetRelativeMouseMode(true);

}

void
graphicsRender() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(cubeProgram.id);

    camera.target = vec3fAdd(camera.position, camera.front);
    Mat4f viewMat = mat4fLookAt(camera.position, camera.target, camera.up);
    glUniformMatrix4fv(cubeProgram.view, 1, false, (GLfloat*)&viewMat);

    Mat4f projectionMat = mat4fPerspective(fovRadians, screenRatio, ZNEAR, ZFAR);
    glUniformMatrix4fv(cubeProgram.projection, 1, false, (GLfloat*)&projectionMat);

    float objectColor[] = {1.0, 0.5, 0.31};
    glUniform3fv(cubeProgram.objectColor, 1, objectColor);

    float lightColor[] = {1.0, 1.0, 1.0};
    glUniform3fv(cubeProgram.lightColor, 1, lightColor);

    glUniform3fv(cubeProgram.lightPosition, 1, (GLfloat*)&lightPos);
    glUniform3fv(cubeProgram.cameraPosition, 1, (GLfloat*)&camera.position);
    /*
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, paddleTexture);
    */


    Vec3f rotate = {
        .x = 1.0,
        .y = 0.0,
        .z = 0.0
    };
    Mat4f modelMat = mat4fIdentity();
    //Vec3f scale = { 0.2, 2.0, 2.0};
    //modelMat = mat4fScale(modelMat, scale);
    //modelMat = mat4fVec3fRotate(modelMat, SDL_GetTicks() * degreesToRadians(-0.1), rotate);
    modelMat = mat4fVec3fTranslate(modelMat, paddle1.position);
    glUniformMatrix4fv(cubeProgram.model, 1, false, (GLfloat*)&modelMat);

    glBindVertexArray(cubeVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUseProgram(lightProgram.id);
    glUniformMatrix4fv(lightProgram.view, 1, false, (GLfloat*)&viewMat);
    glUniformMatrix4fv(lightProgram.projection, 1, false, (GLfloat*)&projectionMat);

    modelMat = mat4fIdentity();
    modelMat = mat4fVec3fTranslate(modelMat, lightPos);
    Vec3f lightScale = {0.2, 0.2, 0.2};
    modelMat = mat4fScale(modelMat, lightScale);
    glUniformMatrix4fv(lightProgram.model, 1, false, (GLfloat*)&modelMat);

    glBindVertexArray(lightVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glUseProgram(0);
    SDL_GL_SwapWindow(window);
}

void
graphicsFree() {
    SDL_DestroyWindow(window);
}
