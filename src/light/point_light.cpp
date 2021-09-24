#include "point_light.h"

PointLight::PointLight(
    glm::vec3 position,
    float constant,
    float linear,
    float quadratic,
    glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular
) : Light(LightType::POINT, ambient, diffuse, specular) {
    POSITION = position;
    CONSTANT = constant;
    LINEAR = linear;
    QUADRATIC = quadratic;
}

glm::vec3 PointLight::get_position() {
    return POSITION;
}

float PointLight::get_constant() {
    return CONSTANT;
}

float PointLight::get_linear() {
    return LINEAR;
}

float PointLight::get_quadratic() {
    return QUADRATIC;
}
