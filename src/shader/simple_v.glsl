#version 330 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout (location = 0) in vec4 vertex;
layout (location = 1) in vec3 color;

out vec3 ourColor;

void main()
{
   gl_Position = projection * view * model * vertex;
   ourColor = color;
}