#version 330 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout (location = 0) in vec4 vertex;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 fColor;
out vec2 fTexCoord;

void main()
{
   gl_Position = projection * view * model * vertex;
   fColor = color;
   fTexCoord = texCoord;
}
