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
#include "defines.h"

#include "3d_data.h"

static int screenWidth = 1200;
static int screenHeight = 1000;
static float screenRatio = 0;
static float fovDegree = 90;
static float fovRadians = 0;

static GLchar *vertexShaderSource[] = {""};
static GLchar *fragmentShaderSource[] = {""};

GLuint cubeVao;
GLuint floorVao;
GLuint vbo;
GLuint floorVbo;
GLuint ibo;
GLuint lightVao;
SDL_Window *window;
GLuint paddleTexture;
GLuint woodTexture0;
GLuint woodTexture1;

void
createTexture(GLuint *texture, char const *fileName, GLenum format);

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
ShadowsDepthProgram shadowsDepthProgram;
GLuint depthCubeMap;
GLuint shadowWidth = 1024;
GLuint shadowHeight = 1024;
GLuint depthMapFbo;
Mat4f identityMat;

void
graphicsInit() {
    identityMat = mat4fIdentity();
    SDL_Log("Loading...");
    screenRatio = (screenWidth * 1.0) / screenHeight;
    fovRadians = degreesToRadians(fovDegree);
    int err = SDL_Init(SDL_INIT_VIDEO);
    if (err) {
      fatalSdlError("SDL_Init");
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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

    glDebugMessageCallback(openglDebugMessageCallback, NULL);

    err = SDL_GL_SetSwapInterval(1);

    if (err) {
      fatalSdlError("SDL_GL_SetSwapInterval");
    }

    cubeProgram = cubeProgramCreate();
    lightProgram = lightProgramCreate();
    shadowsDepthProgram = shadowsDepthProgramCreate();

    glGenFramebuffers(1, &depthMapFbo);

    glGenTextures(1, &depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    for (int i = 0 ; i < 6 ; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

    glVertexAttribPointer(
            cubeProgram.aTexCoords
            , 2
            , GL_FLOAT
            , GL_FALSE
            , 8 * sizeof(float)
            , (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(cubeProgram.aTexCoords);

    glActiveTexture(GL_TEXTURE0);
    createTexture(&paddleTexture, "paddle.png", GL_RGBA);

    glGenVertexArrays(1, &floorVao);
    glGenBuffers(1, &floorVbo);

    glBindVertexArray(floorVao);
    glBindBuffer(GL_ARRAY_BUFFER, floorVbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

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

    glVertexAttribPointer(
            cubeProgram.aTexCoords
            , 2
            , GL_FLOAT
            , GL_FALSE
            , 8 * sizeof(float)
            , (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(cubeProgram.aTexCoords);


    createTexture(&woodTexture0, "wood00.jpg", GL_RGB);

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

    SDL_Log("Loading done.");
}

void
graphicsRender() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // create depth cube transformation matrices
    float nearPlane = 0.1;
    float farPlane = 25.0;
    Mat4f shadowProj = mat4fPerspective(
            degreesToRadians(90.0)
            , (float)shadowWidth / (float)shadowHeight
            , nearPlane
            , farPlane
    );

    // render to depth cubemap
    glViewport(0, 0, shadowWidth, shadowHeight);
    Mat4f shadowTransforms[6];

    Vec3f up0 = {0.0, -1.0, 0.0};
    Vec3f dir0 = {1.0, 0.0, 0.0};
    Vec3f target0 = vec3fAdd(world.ball.position, dir0);
    Mat4f lookAt0 = mat4fLookAt(world.ball.position, target0, up0);
    shadowTransforms[0] = mat4fMulMat4f(shadowProj, lookAt0);

    Vec3f up1  = { 0.0, -1.0,  0.0};
    Vec3f dir1 = {-1.0,  0.0,  0.0};
    Vec3f target1 = vec3fAdd(world.ball.position, dir1);
    Mat4f lookAt1 = mat4fLookAt(world.ball.position, target1, up1);
    shadowTransforms[1] = mat4fMulMat4f(shadowProj, lookAt1);

    Vec3f up2  = { 0.0,  0.0,  1.0};
    Vec3f dir2 = { 0.0,  1.0,  0.0};
    Vec3f target2 = vec3fAdd(world.ball.position, dir2);
    Mat4f lookAt2 = mat4fLookAt(world.ball.position, target2, up2);
    shadowTransforms[2] = mat4fMulMat4f(shadowProj, lookAt2);

    Vec3f up3  = { 0.0,  0.0, -1.0};
    Vec3f dir3 = { 0.0, -1.0,  0.0};
    Vec3f target3 = vec3fAdd(world.ball.position, dir3);
    Mat4f lookAt3 = mat4fLookAt(world.ball.position, target3, up3);
    shadowTransforms[3] = mat4fMulMat4f(shadowProj, lookAt3);

    Vec3f up4  = { 0.0, -1.0,  0.0};
    Vec3f dir4 = { 0.0,  0.0,  1.0};
    Vec3f target4 = vec3fAdd(world.ball.position, dir4);
    Mat4f lookAt4 = mat4fLookAt(world.ball.position, target4, up4);
    shadowTransforms[4] = mat4fMulMat4f(shadowProj, lookAt4);

    Vec3f up5  = { 0.0, -1.0,  0.0};
    Vec3f dir5 = { 0.0,  0.0, -1.0};
    Vec3f target5 = vec3fAdd(world.ball.position, dir5);
    Mat4f lookAt5 = mat4fLookAt(world.ball.position, target5, up5);
    shadowTransforms[5] = mat4fMulMat4f(shadowProj, lookAt5);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(shadowsDepthProgram.id);
    for (int i = 0 ; i < 6 ; i++) {
        glUniformMatrix4fv(
                shadowsDepthProgram.shadowMatrices[i]
                , 1
                , false
                , (GLfloat*)&shadowTransforms[i]
        );
    }
    glUniform1f(shadowsDepthProgram.farPlane, farPlane);
    glUniform3fv(
            shadowsDepthProgram.lightPos
            , 1
            , (GLfloat*)&world.ball.position
    );

    // TODO : remove code duplication from normal rendering
    // the difference between the 2 is the attribute location
    // (for example shadowsDepthProgram.model instead of cubeProgram.model)
    // and here we don't render textures
    glBindVertexArray(cubeVao);
    for (int i = 0 ; i < PADDLES_N ; i++) {
        Paddle paddle = world.paddles[i];
        Mat4f modelMat = mat4fVec3fTranslate(identityMat, paddle.position);
        modelMat = mat4fScale(modelMat, paddle.size);
        glUniformMatrix4fv(shadowsDepthProgram.model, 1, false, (GLfloat*)&modelMat);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(floorVao);
    for (int i = 0 ; i < WALLS_N ; i++) {
        Wall wall = world.walls[i];
        Mat4f modelMat = mat4fVec3fTranslate(identityMat, wall.position);
        modelMat = mat4fScale(modelMat, wall.size);
        glUniformMatrix4fv(shadowsDepthProgram.model, 1, false, (GLfloat*)&modelMat);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // render normally
    glViewport(0, 0, screenWidth, screenHeight);
    glUseProgram(cubeProgram.id);

    glUniform1f(cubeProgram.farPlane, farPlane);
    camera.target = vec3fAdd(camera.position, camera.front);
    Mat4f viewMat = mat4fLookAt(camera.position, camera.target, camera.up);
    glUniformMatrix4fv(cubeProgram.view, 1, false, (GLfloat*)&viewMat);

    Mat4f projectionMat = mat4fPerspective(fovRadians, screenRatio, ZNEAR, ZFAR);
    glUniformMatrix4fv(cubeProgram.projection, 1, false, (GLfloat*)&projectionMat);

    glUniform3fv(cubeProgram.cameraPosition, 1, (GLfloat*)&camera.position);

    glUniform3fv(cubeProgram.pointLight.position, 1, (GLfloat*)&world.ball.position);
    float ambient = 0.1;
    float diffuse = 0.5;
    glUniform3f(cubeProgram.pointLight.ambient, ambient, ambient, ambient);
    glUniform3f(cubeProgram.pointLight.diffuse, diffuse, diffuse, diffuse);
    glUniform3f(cubeProgram.pointLight.specular, 1.0, 1.0, 1.0);
    glUniform1f(cubeProgram.pointLight.constant, 1.0);
    glUniform1f(cubeProgram.pointLight.linear, 0.09);
    glUniform1f(cubeProgram.pointLight.quadratic, 0.032);

    glUniform1i(cubeProgram.material.diffuse, 0);
    glUniform1i(cubeProgram.material.specular, 1);
    glUniform1f(cubeProgram.material.shininess, 64.0);

    glUniform1i(cubeProgram.depthMap, 1);
    glBindVertexArray(cubeVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, paddleTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    for (int i = 0 ; i < PADDLES_N ; i++) {
        Paddle paddle = world.paddles[i];
        Mat4f modelMat = mat4fVec3fTranslate(identityMat, paddle.position);
        modelMat = mat4fScale(modelMat, paddle.size);
        glUniformMatrix4fv(cubeProgram.model, 1, false, (GLfloat*)&modelMat);

        Mat4f modelInverseMat = mat4fInverse(modelMat);
        glUniformMatrix4fv(cubeProgram.modelInverse, 1, false, (GLfloat*)&modelInverseMat);


        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture0);

    for (int i = 0 ; i < WALLS_N ; i++) {
        Wall wall = world.walls[i];
        Mat4f modelMat = mat4fVec3fTranslate(identityMat, wall.position);
        modelMat = mat4fScale(modelMat, wall.size);
        glUniformMatrix4fv(cubeProgram.model, 1, false, (GLfloat*)&modelMat);

        Mat4f modelInverseMat = mat4fInverse(modelMat);
        glUniformMatrix4fv(cubeProgram.modelInverse, 1, false, (GLfloat*)&modelInverseMat);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glUseProgram(lightProgram.id);
    glUniformMatrix4fv(lightProgram.view, 1, false, (GLfloat*)&viewMat);
    glUniformMatrix4fv(lightProgram.projection, 1, false, (GLfloat*)&projectionMat);

    Mat4f lightModelMat = mat4fIdentity();
    lightModelMat = mat4fVec3fTranslate(lightModelMat, world.ball.position);
    //Vec3f lightScale = {0.2, 0.2, 0.2};
    lightModelMat = mat4fScale(lightModelMat, world.ball.size);
    glUniformMatrix4fv(lightProgram.model, 1, false, (GLfloat*)&lightModelMat);
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

void
createTexture(GLuint *texture, char const *fileName, GLenum format) {
    glGenTextures(1, texture);
    SDL_Log("Creating texture %d from %s\n", *texture, fileName);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(fileName, &width, &height, &nrChannels, 0);
    if (!data) {
        fatalError("stbi_load error.\n");
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}
