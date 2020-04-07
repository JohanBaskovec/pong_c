#version 130
in vec3 aPos;
in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelInverse;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    Normal = mat3(transpose(modelInverse)) * aNormal;
    //Normal = mat3(model) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
