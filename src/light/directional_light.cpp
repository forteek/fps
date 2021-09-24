#include "directional_light.h"

DirectionalLight::DirectionalLight(
    glm::vec3 direction,
    glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular
) : Light(LightType::DIRECTIONAL, ambient, diffuse, specular) {
    DIRECTION = direction;
}

glm::vec3 DirectionalLight::get_direction()
{
    return DIRECTION;
}
