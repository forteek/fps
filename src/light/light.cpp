#include "light.h"

Light::Light(LightType type, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
    TYPE = type;
    AMBIENT = ambient;
    DIFFUSE = diffuse;
    SPECULAR = specular;
}

LightType Light::get_type() {
    return TYPE;
}

glm::vec3 Light::get_ambient() {
    return AMBIENT;
}

glm::vec3 Light::get_diffuse() {
    return DIFFUSE;
}

glm::vec3 Light::get_specular() {
    return SPECULAR;
}
