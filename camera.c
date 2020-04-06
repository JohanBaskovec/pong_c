#include "camera.h"
#include <math.h>
#include "maths.h"

Camera camera;

static Vec3f worldUp = {
    .x = 0.0,
    .y = 1.0,
    .z = 0.0
};

void
cameraInit() {
    camera.yaw = -90.0;
    camera.pitch = 0.0;

    camera.position.x = 0.0;
    camera.position.y = 0.0;
    camera.position.z = 3.0;

    camera.front.x = 0.0;
    camera.front.y = 0.0;
    camera.front.z = -1.0;

    camera.up.x = 0.0;
    camera.up.y = 1.0;
    camera.up.z = 0.0;
    updateCameraVector();
}


void
updateCameraVector() {
    float yawRad = degreesToRadians(camera.yaw);
    float pitchRad = degreesToRadians(camera.pitch);
    camera.direction.x = cos(yawRad) * cos(pitchRad);
    camera.direction.y = sin(pitchRad);
    camera.direction.z = sin(yawRad) * cos(pitchRad);

    camera.front = camera.direction;
    vec3fNormalize(&camera.front);

    camera.right = camera.front;
    vec3fCrossProduct(&camera.right, &worldUp);
    vec3fNormalize(&camera.right);

    camera.up = camera.right;
    vec3fCrossProduct(&camera.up, &camera.front);
    vec3fNormalize(&camera.up);
}
