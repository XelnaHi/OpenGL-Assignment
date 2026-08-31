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

#include "glm/fwd.hpp"

class Shader {
public:
    // The program ID
    unsigned int ID;

    // constructor reads and builds shader
    Shader(const char *vertexPath, const char *fragmentPath);

    // use / activate shader
    void use();

    // utility uniform functions
    void setBool(const std::string &name, bool value);

    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3);
    void SetMat4(const std::string &name, unsigned int count, unsigned int normalized, glm::mat4 m4);

    void Unbind();
};


#endif //SHADERCOURSESTARTER_SHADER_H
