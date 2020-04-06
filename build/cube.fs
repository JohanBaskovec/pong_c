#version 130
out vec4 LFragment;
in vec2 TextCoord;
uniform sampler2D paddleTexture;
void main() {
   LFragment = texture(paddleTexture, TextCoord);
}
