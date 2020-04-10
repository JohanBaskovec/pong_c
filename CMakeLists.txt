cmake_minimum_required(VERSION 3.7)

project(Pong)

set(CMAKE_C_STANDARD 11)
add_definitions(
        -D_FORTIFY_SOURCE=2
        -O0
        -Wall
        -Werror
        -Wno-unused-variable
        -g
)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SOURCES
    maths.c
    graphics.c
    input.c
    camera.c
    world.c
    shader.c
    error.c
    mat4f.c
    vec3f.c
    fs.c
    vec4f.c
    ball.c
    paddle.c
    wall.c
)

add_executable(pong
    pong.c
    ${SOURCES}
)
target_link_libraries(pong ${SDL2_LIBRARIES} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} m)
add_executable(pong_test
    maths_test.h
    pong_test.c
    ${SOURCES}
)
target_link_libraries(pong_test ${SDL2_LIBRARIES} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} m)


