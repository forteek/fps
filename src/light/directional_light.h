#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "light.h"

class DirectionalLight : public Light {
    protected:
        glm::vec3 DIRECTION{};

    public:
        DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
        glm::vec3 get_direction() override;
};

#endif
