#include "model.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <map>

using std::map;

map<int, int> CHANNEL_COUNT_FORMATS {
        {1, GL_RED},
        {3, GL_RGB},
        {4, GL_RGBA}
};

Model::Model(const char *path)
{
    load_model(path);
}

void Model::draw(Shader &shader)
{
    for (const Mesh& mesh : MESHES) {
        mesh.draw(shader);
    }
}

void Model::load_model(const string& path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        fprintf(stderr, "%s", importer.GetErrorString());

        return;
    }

    DIRECTORY = path.substr(0, path.find_last_of('/'));

    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        MESHES.push_back(process_mesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene);
    }
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};

        vertex.position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        vertex.normal = glm::vec3(
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z
        );

        if (mesh->mTextureCoords[0]) {
            vertex.textureCoordinates = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        } else {
            vertex.textureCoordinates = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        vector<Texture> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        vector<Texture> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return {vertices, indices, textures};
}

vector<Texture> Model::load_material_textures(aiMaterial *material, aiTextureType type, const string& typeName)
{
    vector<Texture> textures;

    for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
        aiString path;
        material->GetTexture(type, i, &path);

        bool skip = false;
        for (Texture &texture : LOADED_TEXTURES) {
            if (std::strcmp(texture.path.data(), path.C_Str()) == 0) {
                textures.push_back(texture);
                skip = true;
                break;
            }
        }

        if (skip) {
            continue;
        }

        Texture texture;
        texture.id = texture_from_file(path.C_Str(), DIRECTORY);
        texture.type = typeName;
        texture.path = path.C_Str();
        textures.push_back(texture);
        LOADED_TEXTURES.push_back(texture);
        fprintf(stdout, "Loaded %s with id %d\n", path.C_Str(), texture.id);
    }
    return textures;
}

unsigned int Model::texture_from_file(const char *path, const string &directory)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int texture;
    glGenTextures(1, &texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int textureWidth, textureHeight, channelCount;
    unsigned char *data = stbi_load(filename.c_str(), &textureWidth, &textureHeight, &channelCount, 0);
    if (!data) {
        fprintf(stderr, "Failed to load texture.");
        stbi_image_free(data);

        return texture;
    }

    GLint format = CHANNEL_COUNT_FORMATS[channelCount];

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture;
}

