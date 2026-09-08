//
// Created by Teo on 2026-09-03.
//

#ifndef SHADERCOURSESTARTER_MESH_H
#define SHADERCOURSESTARTER_MESH_H
#include <string>

#include "assimp/types.h"
#include "glad/gl.h"
#include "glm/glm.hpp"

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

    void drawInstanced(Shader &shader, unsigned int instanceCount);
    void setupInstancedBuffer(size_t maxInstances);
    void updateInstancedBuffer(const std::vector<glm::mat4> &models);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int instancedVBO = 0;

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
