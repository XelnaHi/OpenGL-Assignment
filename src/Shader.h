//
// Created by Teo on 2026-08-29.
//

#ifndef SHADERCOURSESTARTER_SHADER_H
#define SHADERCOURSESTARTER_SHADER_H

#include <glad/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glm/glm.hpp"

class Shader {
public:
    // The program (shader) ID
    unsigned int ID;

    // constructor reads and builds shader
    Shader(const char *vertexPath, const char *fragmentPath);

    // use / activate shader
    void use();

    // utility uniform functions
    void setBool(const std::string &name, bool value);

    void setInt(const std::string &name, int value);

    void setFloat(const std::string &name, float value);

    void setVec3(const std::string &name, glm::vec3 value);

    void setUniform4f(const std::string &name, float v0, float v1, float v2, float v3);

    void setMat4(const std::string &name, glm::mat4 m4);

    // light settings
    void setDirectionalLight(glm::vec3 direction = glm::vec3(1.0f), glm::vec3 ambient = glm::vec3(1.0f),
                             glm::vec3 diffuse = glm::vec3(1.0f), glm::vec3 specular = glm::vec3(1.0f));

    void setPointLight(unsigned int index, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                       float constant, float linear, float quadratic);

    void setSpotLight(glm::vec3 position, glm::vec3 direction, float cutOff, float outerCutOff,
                  glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                  float constant, float linear, float quadratic);

    void Unbind();
};


#endif //SHADERCOURSESTARTER_SHADER_H
