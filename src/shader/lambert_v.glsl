#version 330

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec4 color=vec4(1, 1, 1, 1);
uniform vec4 lightDir=vec4(0, 0, 1, 0);

layout (location=0) in vec4 vertex;
layout (location=1) in vec4 normal;

out vec4 i_color;

void main(void) {
    gl_Position = projection * view * model * vertex;

    mat4 G = mat4(inverse(transpose(mat3(model))));
    vec4 n = normalize(view * G * normal);

    float nl = clamp(dot(n, lightDir), 0, 1);

    i_color = vec4(color.rgb * nl, color.a);
}
