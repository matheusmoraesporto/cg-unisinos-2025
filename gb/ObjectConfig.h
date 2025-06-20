#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texcoord;
};

struct Matrix
{
    float x, y, z, scale;
};

struct Object3D
{
    // Propriedades do arquivo de configuração
    std::string name;
    std::string objPath;
    std::string mtlPath;
    std::string texturePath;
    Matrix initialPosition;
    Matrix initialRotation;

    // Propriedades do objeto 3D lidas do arquivo OBJ/MTL
    std::vector<GLfloat> positions;
    std::vector<GLfloat> textureCoords;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> ka;
    std::vector<GLfloat> ks;
    float ns;

    std::vector<Vertex> vertices;
    std::vector<glm::u32vec3> triangle_indices;

    // Propriedades do objeto 3D definidos em runtime
    GLuint texID;
    GLuint VAO;
    glm::mat4 model;
    GLint modelLoc;
    bool isSelected;
};

std::vector<Object3D> loadObjectConfigs(const std::string &configPath, const std::string &rootDir);