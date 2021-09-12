#version 330 core

out vec4 FragColor;

in vec3 fColor;
in vec2 fTexCoord;

uniform vec3 lightColor;
uniform sampler2D textureFile;

void main()
{
   FragColor = texture(textureFile, fTexCoord) * vec4(lightColor, 1.0);
}
