
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "stb_image.h"
#include "ObjectConfig.h"
using namespace glm;

void loadOBJ(Object3D *object)
{
    std::vector<glm::vec3> vertexIndices;
    std::vector<glm::vec2> textureIndices;
    std::vector<glm::vec3> normalIndices;

    std::ifstream file(object->objPath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << object->objPath << std::endl;
        return;
    }

    std::string line;
    while (getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            vertexIndices.push_back(glm::vec3(x, y, z));
        }
        else if (prefix == "vt")
        {
            float u, v;
            iss >> u >> v;
            textureIndices.push_back(glm::vec2(u, v));
        }
        else if (prefix == "vn")
        {
            float x, y, z;
            iss >> x >> y >> z;
            normalIndices.push_back(glm::vec3(x, y, z));
        }
        else if (prefix == "f")
        {
            std::string v1, v2, v3;
            iss >> v1 >> v2 >> v3;

            glm::ivec3 vIndices, tIndices, nIndices;
            std::istringstream(v1.substr(0, v1.find('/'))) >> vIndices.x;
            std::istringstream(v1.substr(v1.find('/') + 1, v1.rfind('/') - v1.find('/') - 1)) >> tIndices.x;
            std::istringstream(v1.substr(v1.rfind('/') + 1)) >> nIndices.x;
            std::istringstream(v2.substr(0, v2.find('/'))) >> vIndices.y;
            std::istringstream(v2.substr(v2.find('/') + 1, v2.rfind('/') - v2.find('/') - 1)) >> tIndices.y;
            std::istringstream(v2.substr(v2.rfind('/') + 1)) >> nIndices.y;
            std::istringstream(v3.substr(0, v3.find('/'))) >> vIndices.z;
            std::istringstream(v3.substr(v3.find('/') + 1, v3.rfind('/') - v3.find('/') - 1)) >> tIndices.z;
            std::istringstream(v3.substr(v3.rfind('/') + 1)) >> nIndices.z;

            for (int i = 0; i < 3; i++)
            {
                const glm::vec3 &vertex = vertexIndices[vIndices[i] - 1];
                const glm::vec2 &texture = textureIndices[tIndices[i] - 1];
                const glm::vec3 &normal = normalIndices[nIndices[i] - 1];

                object->positions.push_back(vertex.x);
                object->positions.push_back(vertex.y);
                object->positions.push_back(vertex.z);
                object->textureCoords.push_back(texture.x);
                object->textureCoords.push_back(texture.y);
                object->normals.push_back(normal.x);
                object->normals.push_back(normal.y);
                object->normals.push_back(normal.z);
            }
        }
    }

    file.close();
}

void loadMTL(Object3D *object)
{
    std::string line, readValue;
    std::ifstream mtlFile(object->mtlPath);

    std::cout << "Loading MTL file: " << object->mtlPath << std::endl;

    if (!mtlFile.is_open())
    {
        std::cerr << "Failed to open file: " << object->mtlPath << std::endl;
        return;
    }

    while (getline(mtlFile, line))
    {
        std::istringstream iss(line);

        if (line.find("Ka") == 0)
        {
            float ka1, ka2, ka3;
            iss >> readValue >> ka1 >> ka2 >> ka3;
            object->ka.push_back(ka1);
            object->ka.push_back(ka2);
            object->ka.push_back(ka3);
        }
        else if (line.find("Ks") == 0)
        {
            float ks1, ks2, ks3;
            iss >> readValue >> ks1 >> ks2 >> ks3;
            object->ks.push_back(ks1);
            object->ks.push_back(ks2);
            object->ks.push_back(ks3);
        }
        else if (line.find("Ns") == 0)
        {
            iss >> readValue >> object->ns;
        }
    }

    mtlFile.close();
}

GLuint loadTexture(std::string filePath)
{
    int width, height;
    GLuint texID; // id da textura a ser carregada

    // Gera o identificador da textura na memória
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Ajuste dos parâmetros de wrapping e filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    // Carregamento da imagem usando a função stbi_load da biblioteca stb_image
    int nrChannels;

    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3) // jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else // assume que é 4 canais png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture " << filePath << std::endl;
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}
