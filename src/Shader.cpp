//
// Created by Teo on 2026-08-29.
//

#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

#include "glm/gtc/type_ptr.hpp"

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    // retrieve the vertex/fragment shader source code from filepath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // allow ifstream objects to throw exceptions
    // bitwise operator | is equivalent to: each corresponding bit in an operation becomes = 1 if one of the compare bits is 1.
    // Example: 1001 | 1101 = 1101;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        // read file buffer contents into streams
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // close file handlers
        vShaderFile.close();
        fShaderFile.close();

        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();


    // compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // print potential errors
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    // print potential errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // setup Shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertex),
            glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // print link errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FA    ILED\n" << infoLog << std::endl;
    }

    // shaders have now been linked to program and can safely be discarded
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, glm::vec3 value) {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setUniform4f(const std::string &name, float v0, float v1, float v2, float v3) {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2, v3);
}

void Shader::setMat4(const std::string &name, glm::mat4 m4) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m4));
}

void Shader::setDirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
    glUniform3f(glGetUniformLocation(ID, "u_DirLight.direction"), direction.x, direction.y, direction.z);
    glUniform3f(glGetUniformLocation(ID, "u_DirLight.ambient"), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(ID, "u_DirLight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(ID, "u_DirLight.specular"), specular.x, specular.y, specular.z);
}

void Shader::setPointLight(unsigned int index, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse,
                           glm::vec3 specular, float constant, float linear, float quadratic) {
    // vectors
    std::string positionFormat = "u_PointLights[" + std::to_string(index) + "].position";
    std::string ambientFormat = "u_PointLights[" + std::to_string(index) + "].ambient";
    std::string diffuseFormat = "u_PointLights[" + std::to_string(index) + "].diffuse";
    std::string specularFormat = "u_PointLights[" + std::to_string(index) + "].specular";

    glUniform3f(glGetUniformLocation(ID, positionFormat.c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(ID, ambientFormat.c_str()), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(ID, diffuseFormat.c_str()), diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(ID, specularFormat.c_str()), specular.x, specular.y, specular.z);

    // floats
    std::string constantFormat = "u_PointLights[" + std::to_string(index) + "].constant";
    std::string linearFormat = "u_PointLights[" + std::to_string(index) + "].linear";
    std::string quadraticFormat = "u_PointLights[" + std::to_string(index) + "].quadratic";

    glUniform1f(glGetUniformLocation(ID, constantFormat.c_str()), constant);
    glUniform1f(glGetUniformLocation(ID, linearFormat.c_str()), linear);
    glUniform1f(glGetUniformLocation(ID, quadraticFormat.c_str()), quadratic);
}

void Shader::setSpotLight(glm::vec3 position, glm::vec3 direction, float cutOff, float outerCutOff,
                          glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                          float constant, float linear, float quadratic) {
    glUniform3f(glGetUniformLocation(ID, "u_SpotLight.position"), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(ID, "u_SpotLight.direction"), direction.x, direction.y, direction.z);
    glUniform1f(glGetUniformLocation(ID, "u_SpotLight.cutOff"), cutOff);
    glUniform1f(glGetUniformLocation(ID, "u_SpotLight.outerCutOff"), outerCutOff);

    glUniform3f(glGetUniformLocation(ID, "u_SpotLight.ambient"), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(ID, "u_SpotLight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(ID, "u_SpotLight.specular"), specular.x, specular.y, specular.z);

    glUniform1f(glGetUniformLocation(ID, "u_SpotLight.constant"), constant);
    glUniform1f(glGetUniformLocation(ID, "u_SpotLight.linear"), linear);
    glUniform1f(glGetUniformLocation(ID, "u_SpotLight.quadratic"), quadratic);
}

void Shader::Unbind() {
    glDeleteProgram(ID);
}
