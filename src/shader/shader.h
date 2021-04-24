#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using std::string;
using std::ifstream;
using std::stringstream;

class Shader
{
    private:
        unsigned int ID;
        void checkCompileErrors(uint shader, string type);

    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        void use();
        GLint uniform(string name);
        GLint attribute(string name);
};

#endif
