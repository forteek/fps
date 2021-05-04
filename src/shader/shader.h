#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using std::string;
using std::ifstream;
using std::stringstream;

class Shader
{
    private:
        unsigned int ID;
        static void checkCompileErrors(uint shader, const string& type);

    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        void use() const;
        GLint uniform(const string& name) const;
        GLint attribute(const string& name) const;
        void setUniformMatrix(const string& name, glm::mat4 value) const;
};

#endif
