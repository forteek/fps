#version 330 core

struct Light {
    int type;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 viewPos;
uniform int lightsCount;
uniform Light lights[1];

vec3 calc_directional_light(Light light, vec3 normal, vec3 viewDir);
vec3 calc_point_light(Light light, vec3 normal, vec3 viewDir, vec3 fragPos);

void main()
{
    vec3 normalized = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    for (int i = 0; i < lights.length(); i++) {
        switch (lights[i].type) {
            case 0:
            result += calc_directional_light(lights[i], normalized, viewDir);
            break;
            case 1:
            result += calc_point_light(lights[i], normalized, FragPos, viewDir);
            break;
        }
    }

    FragColor = vec4(result, 1.0);
}

vec3 calc_directional_light(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

    return (ambient + diffuse + specular);
}

vec3 calc_point_light(Light light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}
