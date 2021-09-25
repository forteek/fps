#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../shader/shader.h"
#include "mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using std::vector;
using std::string;

class Model
{
    private:
        vector<Mesh> MESHES;
        string DIRECTORY;

        void load_model(const string& path);
        void process_node(aiNode *node, const aiScene *scene);
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, const string& typeName);
        static unsigned int texture_from_file(const char *path, const string &directory);
    public:
        explicit Model(const char *path);
        void draw(Shader &shader);
};

#endif
