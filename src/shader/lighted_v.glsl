#version 330 core

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0);
    FragPos = vec3(model * vec4(vertex, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;
}