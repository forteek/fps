#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform float currentTime;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 wavy_pos = vec3(
        aPos.x + (sin(currentTime + aPos.y)) / 10,
        aPos.y,
        aPos.z
    );
    FragPos = vec3(model * vec4(wavy_pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(wavy_pos, 1.0);
}