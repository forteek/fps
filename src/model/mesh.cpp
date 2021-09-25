#include <utility>
#include <iostream>
#include <map>
#include "mesh.h"

using std::map;

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
    VERTICES = std::move(vertices);
    INDICES = std::move(indices);
    TEXTURES = std::move(textures);

    setup_mesh();
}

void Mesh::setup_mesh()
{
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBufferData(GL_ARRAY_BUFFER, VERTICES.size() * sizeof(Vertex), &VERTICES[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDICES.size() * sizeof(unsigned int), &INDICES[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoordinates));

    glBindVertexArray(0);
}

void Mesh::draw(Shader &shader) const
{
    map<string, int> indices {
        {"texture_diffuse", 1},
        {"texture_specular", 1},
        {"texture_normal", 1},
        {"texture_height", 1},
    };

    for (const Texture& texture : TEXTURES) {
        glActiveTexture(GL_TEXTURE0 + texture.id - 1);

        string number = std::to_string(indices[texture.type]++);
        shader.setUniformInt(texture.type + number, texture.id - 1);

        glBindTexture(GL_TEXTURE_2D, texture.id);
    }

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, INDICES.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
