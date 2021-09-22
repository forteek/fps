#include "shader.h"

unsigned int ID;

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    string vertexCode, fragmentCode;

    try {
        ifstream vShaderFile, fShaderFile;
        vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (ifstream::failure& error) {
        fprintf(stderr, "Failed to initialize shader. %s", error.what());
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertexShader, fragmentShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    Shader::checkCompileErrors(vertexShader, "VERTEX");

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    Shader::checkCompileErrors(fragmentShader, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    Shader::checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::checkCompileErrors(unsigned int shader, const string& type)
{
    int success;
    char log[1024];

    if (type == "PROGRAM") {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, log);
            fprintf(stderr, "Program linking error compilation error - %s\n%s", type.c_str(), log);
        }

        return;
    }

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        fprintf(stderr, "Shader compilation error - %s\n%s", type.c_str(), log);
    }
}

GLint Shader::uniform(const string& name) const {
    return glGetUniformLocation(ID, name.c_str());
}

GLint Shader::attribute(const string& name) const {
    return glGetAttribLocation(ID, name.c_str());
}

void Shader::setUniformMatrix(const string& name, glm::mat4 value) const {
    glUniformMatrix4fv(this->uniform(name), 1, false, glm::value_ptr(value));
}

void Shader::setUniformVec3(const string& name, glm::vec3 value) const {
    glUniform3fv(this->uniform(name), 1, glm::value_ptr(value));
}

void Shader::setUniformFloat(const string& name, float value) const {
    glUniform1f(this->uniform(name), value);
}
