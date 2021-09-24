#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include "light_type.h"

class Light {
    protected:
        LightType TYPE;
        glm::vec3 AMBIENT{}, DIFFUSE{}, SPECULAR{};

    public:
        Light(LightType type, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
        LightType get_type();
        glm::vec3 get_ambient();
        glm::vec3 get_diffuse();
        glm::vec3 get_specular();
};

#endif
