//
// Created by Teo on 2026-09-03.
//

#include "Mesh.h"

#include <utility>

#include "Shader.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)) {
    setupMesh();
}

void Mesh::draw(Shader shader) {

    /* Below is a flexible way of deducing the amount of specific texture types. It operates on a strict assumption that texture uniforms follow the following naming convention;
    uniform sampler2D texture_diffuse1;
    uniform sampler2D texture_diffuse2;
    uniform sampler2D texture_diffuse3;
    uniform sampler2D texture_diffuseN...;
    uniform sampler2D texture_specular1;
    uniform sampler2D texture_specular2;
    uniform sampler2D texture_specularN...;
*/
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++) {

        glActiveTexture(GL_TEXTURE0 + i); // selects a slot, for example slot 3.

        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse") {

            diffuseNr++;
            number = std::to_string(diffuseNr);
        } else if (name == "texture_specular") {
            number = std::to_string(specularNr++);
        }

        shader.setInt(("u_Material." + name + number).c_str(), i); // number here can be whatever, depending on the order of declarations and the amount of declared texture uniforms, following the above naming conventions. The uniform gets assigned the texture slot id, which tells the (for example) sampler2D uniform to read from that particular texture slot.
        glBindTexture(GL_TEXTURE_2D, textures[i].id); // we store in the texture slot an id that belongs to a specific texture object containing the texture data, such as pixel data, mipmaps and format.
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0); // enabling a vertex slot means data from that particular layout will be read and used. If not enabled, constant pre-defined values will be used. Useful flexibility in case a part of the memory, such as the color attributes, do not differ in values, and thus can rely on constant values instead of storing data for it.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // One configuration set on the VAO on how to interpret the memory allocated in the VertexBufferObject. Below, we set more configurations. If we omit further instructions, the GPU will only know how to interpret first three values of each vertex.

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)); // offsetoff(container, member) is a useful way to find the distance in memory from the beginning of a container object layout to that of the specified member.

    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0); // Memory layout has been allocated along with memory interpretation instructions. We set the currently bound vertex array object to "0", ie the default one. We do not expect to use this, as part of our setup of VAOs, VBOs and EBOs means creating unique id handles.
}
