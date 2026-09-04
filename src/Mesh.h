//
// Created by Teo on 2026-09-03.
//

#ifndef SHADERCOURSESTARTER_MESH_H
#define SHADERCOURSESTARTER_MESH_H
#include <string>

#include "assimp/types.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class Shader;
struct Vertex;
struct Texture;

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void draw(Shader &shader);

private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

struct Vertex {
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Normal = glm::vec3(0.0f);
    glm::vec2 TexCoords = glm::vec2(0.0f);
};

struct Texture {
    unsigned int id;
    std::string type;
    aiString path;
};


#endif //SHADERCOURSESTARTER_MESH_H
