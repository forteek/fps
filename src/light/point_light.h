#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "light.h"

class PointLight : public Light {
    protected:
        glm::vec3 POSITION{};
        float CONSTANT, LINEAR, QUADRATIC;

    public:
        PointLight(
            glm::vec3 position,
            float constant,
            float linear,
            float quadratic,
            glm::vec3 ambient,
            glm::vec3 diffuse,
            glm::vec3 specular
        );
        glm::vec3 get_position() override;
        float get_constant() override;
        float get_linear() override;
        float get_quadratic() override;
};

#endif
