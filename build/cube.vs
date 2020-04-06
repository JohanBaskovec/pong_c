#version 130
in vec3 LVertexPos2D;
in vec2 inTextureCoord;
out vec2 TextCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
   gl_Position = projection * view * model * vec4(LVertexPos2D, 1.0f);
   TextCoord = inTextureCoord;
}
