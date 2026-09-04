//
// Created by Teo on 2026-09-03.
//

#ifndef SHADERCOURSESTARTER_MODEL_H
#define SHADERCOURSESTARTER_MODEL_H
#include <string>
#include <vector>

#include "Mesh.h"
#include "assimp/scene.h"
#include "Shader.h"

struct Texture;

class Model {
public:
    std::vector<Texture> textures_loaded;

    Model() = default;

    Model(const char *path);

    void draw(Shader &shader);

private:
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path);

    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);

};


#endif //SHADERCOURSESTARTER_MODEL_H
