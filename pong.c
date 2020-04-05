#include <SDL.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdbool.h>
#include "maths.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint createShader(
        GLenum shaderType
        , const GLchar * const *source
        , GLuint program
);

typedef struct Paddle {
    Vec3f position;

} Paddle;

Paddle paddle1;
Paddle paddle2;

int screenWidth = 800;
int screenHeight = 600;
float screenRatio = 0;
float fovDegree = 90;
float fovRadians = 0;
float movementSpeed = 0.1;

#define zNear 0.1
#define zFar 100.0

const GLchar *vertexShaderSource[] = {
    "#version 130\n"
    "in vec3 LVertexPos2D;\n"
    "in vec2 inTextureCoord;\n"
    "out vec2 TextCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "   gl_Position = projection * view * model * vec4(LVertexPos2D, 1.0f);\n"
    "   TextCoord = inTextureCoord;\n"
    "}"
};

const GLchar *fragmentShaderSource[] = {
    "#version 130\n"
    "out vec4 LFragment;\n"
    "in vec2 TextCoord;\n"
    "uniform sampler2D paddleTexture;\n"
    "void main() {\n"
    "   LFragment = texture(paddleTexture, TextCoord);\n"
    //"   LFragment = vec4(1.0,1.0,1.0,1.0);\n"
    "}"
};

// 3D model of paddle
GLfloat vertices[] =  {
    // positions          // texture
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
GLuint indices[] = {
    0, 1, 3,
    1, 2, 3,
};

void fatalError(const char *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, format, argptr);
    va_end(argptr);
    exit(1);
}

void fatalSdlError(const char* funcName) {
    const char *sdlError = SDL_GetError();
    if (sdlError == NULL) {
        sdlError = "No error message from SDL.";
    }
    fatalError("%s: fatal error: %s\n", funcName, sdlError);
}

typedef struct PressedKeys {
    bool escape;
    bool up;
    bool down;
    bool cameraUp;
    bool cameraDown;
    bool cameraLeft;
    bool cameraRight;
} PressedKeys;

PressedKeys pressedKeys;

void keyDown(SDL_KeyboardEvent *e) {
    SDL_Log("Pressed key\n");
    switch (e->keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
            pressedKeys.escape = true;
            break;
        case SDL_SCANCODE_S:
            pressedKeys.down = true;
            break;
        case SDL_SCANCODE_W:
            pressedKeys.up = true;
            break;
        case SDL_SCANCODE_UP:
            pressedKeys.cameraUp = true;
            break;
        case SDL_SCANCODE_RIGHT:
            pressedKeys.cameraRight = true;
            break;
        case SDL_SCANCODE_DOWN:
            pressedKeys.cameraDown = true;
            break;
        case SDL_SCANCODE_LEFT:
            pressedKeys.cameraLeft = true;
            break;
        default:
            SDL_Log("Unsupported key pressed.\n");
    }
}

void keyUp(SDL_KeyboardEvent *e) {
    switch (e->keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
            pressedKeys.escape = false;
            break;
        case SDL_SCANCODE_S:
            pressedKeys.down = false;
            break;
        case SDL_SCANCODE_W:
            pressedKeys.up = false;
            break;
        case SDL_SCANCODE_UP:
            pressedKeys.cameraUp = false;
            break;
        case SDL_SCANCODE_RIGHT:
            pressedKeys.cameraRight = false;
            break;
        case SDL_SCANCODE_DOWN:
            pressedKeys.cameraDown = false;
            break;
        case SDL_SCANCODE_LEFT:
            pressedKeys.cameraLeft = false;
            break;
        default:
            SDL_Log("Unsupported key pressed.\n");
    }
}

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

typedef struct Camera {
    Vec3f position;
    Vec3f direction;
    Vec3f target;
    Vec3f right;
    Vec3f up;
} Camera;

Camera camera;

int main(int argc, char *argv[])
{
    camera.position.x = 0.0;
    camera.position.y = 0.0;
    camera.position.z = 6.0;

    camera.target.x = 0.0;
    camera.target.y = 0.0;
    camera.target.z = 0.0;

    camera.direction = camera.position;
    vec3fSub(&camera.direction, &camera.target);
    vec3fNormalize(&camera.direction);

    camera.up.x = 0.0;
    camera.up.y = 1.0;
    camera.up.z = 0.0;

    camera.right = camera.up;
    vec3fCrossProduct(&camera.right, &camera.direction);
    vec3fNormalize(&camera.right);

    vec3fCrossProduct(&camera.direction, &camera.right);

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


    SDL_Window *window = SDL_CreateWindow(
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

    GLuint program = glCreateProgram();

    GLuint vertexShader = createShader(
            GL_VERTEX_SHADER
            , vertexShaderSource
            , program
    );

    GLuint fragmentShader = createShader(
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

    GLint inTextureCoordVar = glGetAttribLocation(program, "inTextureCoord");
    GLint vertexVar = glGetAttribLocation(program, "LVertexPos2D");
    GLint modelVar = glGetUniformLocation(program, "model");
    GLint projectionVar = glGetUniformLocation(program, "projection");
    GLint viewVar = glGetUniformLocation(program, "view");
    GLint paddleTextureVar = glGetUniformLocation(program, "paddleTexture");

    GLuint vao;
    GLuint vbo;
    GLuint ibo;

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

    GLuint paddleTexture;
    glGenTextures(1, &paddleTexture);

    glBindTexture(GL_TEXTURE_2D, paddleTexture);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

    SDL_Event e;

    SDL_Log("OpengL: available image units = %d\n",  GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

    while (true) {
        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        Mat4f viewMat;
        float radius = 5.0;
        float ticks = SDL_GetTicks();
        float camX = sin(ticks / 5000.0) * radius;
        float camZ = cos(ticks / 5000.0) * radius;
        Vec3f camera2 = {
            .x = camX,
            .y = 0.0,
            .z = camZ
        };
        Vec3f target2 = {
            .x = 0.0,
            .y = 0.0,
            .z = 0.0
        };
        Vec3f up2 = {
            .x = 0.0,
            .y = 1.0,
            .z = 0.0
        };
        //Mat4f view;
        mat4fLookAt(&viewMat, &camera2, &target2, &up2);
        glUniformMatrix4fv(viewVar, 1, false, (GLfloat*)&viewMat);

        Mat4f projectionMat;
        mat4fPerspective(&projectionMat, fovRadians, screenRatio, zNear, zFar);
        glUniformMatrix4fv(projectionVar, 1, false, (GLfloat*)&projectionMat);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, paddleTexture);

        glBindVertexArray(vao);

        Vec3f rotate = {
            .x = 1.0,
            .y = 0.0,
            .z = 0.0
        };
        Mat4f modelMat;
        mat4fIdentity(&modelMat);
        mat4fVec3fRotate(&modelMat, SDL_GetTicks() * degreesToRadians(-0.1), &rotate);
        mat4fVec3fTranslate(&modelMat, &paddle1.position);
        glUniformMatrix4fv(modelVar, 1, false, (GLfloat*)&modelMat);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glUseProgram(0);
        SDL_GL_SwapWindow(window);

        SDL_PollEvent(&e);
        switch (e.type) {
            case SDL_QUIT:
                goto end;
                break;
            case SDL_KEYDOWN:
                keyDown(&e.key);
                break;
            case SDL_KEYUP:
                keyUp(&e.key);
                break;
            default:
                // do nothing
                break;
        }

        if (pressedKeys.escape) {
            goto end;
        }
        if (pressedKeys.up) {
            paddle1.position.y += movementSpeed;
        } else if (pressedKeys.down) {
            paddle1.position.y -= movementSpeed;
        }
        if (pressedKeys.cameraUp) {
            camera.position.y += 0.2;
        } else if (pressedKeys.cameraDown) {
            camera.position.y -= 0.2;
        } else if (pressedKeys.cameraLeft) {
            camera.position.x -= 0.2;
        } else if (pressedKeys.cameraRight) {
            camera.position.x += 0.2;
        }

    }

end:

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

GLuint createShader(
        GLenum shaderType
        , const GLchar * const *source
        , GLuint program
) {
    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, source, NULL);

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

