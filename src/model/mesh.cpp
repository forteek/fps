#include <utility>
#include "mesh.h"

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
    int diffuseIndex = 1;
    int specularIndex = 1;
    int index = 0;

    for (const Texture& texture : TEXTURES) {
        glActiveTexture(GL_TEXTURE0 + index);

        string number;

        if (TEXTURES[index].type == "texture_diffuse") {
            number = std::to_string(diffuseIndex++);
        } else if (TEXTURES[index].type == "texture_specular") {
            number = std::to_string(specularIndex++);
        }

        shader.setUniformInt("material." + texture.type + number, index);
        glBindTexture(GL_TEXTURE_2D, texture.id);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, INDICES.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
