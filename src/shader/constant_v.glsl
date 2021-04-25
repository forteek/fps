#version 330

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout (location=0) in vec4 vertex;
layout (location=1) in vec4 normal;

void main(void) {
    gl_Position = projection * view * model * vertex;
}
