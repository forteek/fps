#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <glad/glad.h>
#include "../shader/shader.h"

using std::vector;
using std::string;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoordinates;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
    private:
        vector<Vertex>       VERTICES;
        vector<unsigned int> INDICES;
        vector<Texture>      TEXTURES;
        unsigned int VAO{}, VBO{}, EBO{};

        void setup_mesh();

    public:
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        void draw(Shader &shader) const;
};

#endif
