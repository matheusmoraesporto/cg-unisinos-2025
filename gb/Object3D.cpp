#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "stb_image.h"

#include "Object3D.h"

void Object3D::initialize(
    std::string name,
    std::string objPath,
    std::string mtlPath,
    std::string texturePath,
    float scale,
    glm::vec3 position,
    glm::vec3 rotation,
    std::vector<glm::vec3> trajectoryPoints,
    float trajectorySpeed,
    Shader *shader)
{
    this->name = name;
    this->objPath = objPath;
    this->mtlPath = mtlPath;
    this->texturePath = texturePath;
    this->scale = scale;
    this->position = position;
    this->rotation = rotation;
    this->shader = shader;
    this->trajectoryPoints = trajectoryPoints;
    this->model = glm::mat4(1.0f); // matriz identidade
    this->currentPointIndex = 0;
    this->trajectoryT = 0.0f;
    this->trajectorySpeed = trajectorySpeed;
}

void Object3D::initialSetup()
{
    loadOBJ();
    loadMTL();
    loadTexture();
    setupGeometry();

    // Enviar a informação de qual variável armazenará o buffer da textura
    glUniform1i(glGetUniformLocation(shader->ID, "texBuff"), 0);

    glUniform1f(glGetUniformLocation(shader->ID, "ka"), ka[0]);
    glUniform1f(glGetUniformLocation(shader->ID, "kd"), 0.5);
    glUniform1f(glGetUniformLocation(shader->ID, "ks"), ks[0]);
    glUniform1f(glGetUniformLocation(shader->ID, "q"), ns);

    // Ativando o primeiro buffer de textura da OpenGL
    glActiveTexture(GL_TEXTURE0);

    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, value_ptr(model));
}

void Object3D::setupGeometry()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO[3];
    glGenBuffers(3, VBO);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    // Texture Coords
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), textureCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(1);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Object3D::transform(
    bool isRotating,
    bool isTranslating,
    bool isCurving,
    bool actionW,
    bool actionS,
    bool actionA,
    bool actionD,
    bool actionI,
    bool actionJ,
    bool incrementScale,
    bool decrementScale)
{
    if (isSelected)
    {
        if (isRotating)
        {
            rotate(actionW, actionS, actionA, actionD, actionI, actionJ);
        }

        if (isTranslating)
        {
            translate(actionW, actionS, actionA, actionD, actionI, actionJ);
        }

        if (incrementScale)
        {
            scale *= 1.1f;
        }

        if (decrementScale)
        {
            scale *= 0.9f;
        }

        if (isCurving)
        {
            updateTrajectory();
        }

    }

    updateModelMatrix();
}

void Object3D::draw()
{
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texID);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, (positions.size() / 3));
    glBindVertexArray(0);
}

void Object3D::translate(
    bool actionW,
    bool actionS,
    bool actionA,
    bool actionD,
    bool actionI,
    bool actionJ)
{
    glm::vec3 delta(0.0f);

    if (actionW)
        delta.y += 0.1f;
    if (actionS)
        delta.y -= 0.1f;
    if (actionA)
        delta.x -= 0.1f;
    if (actionD)
        delta.x += 0.1f;
    if (actionJ)
        delta.z += 0.1f;
    if (actionI)
        delta.z -= 0.1f;

    position += delta;
}

void Object3D::updateModelMatrix()
{
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, glm::vec3(scale, scale, scale));
}

void Object3D::rotate(
    bool actionW,
    bool actionS,
    bool actionA,
    bool actionD,
    bool actionI,
    bool actionJ)
{
    if (actionW)
        rotation.x -= 0.9f;
    if (actionS)
        rotation.x += 0.9f;
    if (actionA)
        rotation.y -= 0.9f;
    if (actionD)
        rotation.y += 0.9f;
    if (actionI)
        rotation.z -= 0.9f;
    if (actionJ)
        rotation.z += 0.9f;
}

void Object3D::loadOBJ()
{
    std::vector<glm::vec3> vertexIndices;
    std::vector<glm::vec2> textureIndices;
    std::vector<glm::vec3> normalIndices;

    std::ifstream file(objPath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << objPath << std::endl;
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

                positions.push_back(vertex.x);
                positions.push_back(vertex.y);
                positions.push_back(vertex.z);
                textureCoords.push_back(texture.x);
                textureCoords.push_back(texture.y);
                normals.push_back(normal.x);
                normals.push_back(normal.y);
                normals.push_back(normal.z);
            }
        }
    }

    file.close();
}

void Object3D::loadMTL()
{
    std::string line, readValue;
    std::ifstream mtlFile(mtlPath);
    if (!mtlFile.is_open())
    {
        std::cerr << "Failed to open file: " << mtlPath << std::endl;
        return;
    }

    while (getline(mtlFile, line))
    {
        std::istringstream iss(line);

        if (line.find("Ka") == 0)
        {
            float ka1, ka2, ka3;
            iss >> readValue >> ka1 >> ka2 >> ka3;
            ka.push_back(ka1);
            ka.push_back(ka2);
            ka.push_back(ka3);
        }
        else if (line.find("Ks") == 0)
        {
            float ks1, ks2, ks3;
            iss >> readValue >> ks1 >> ks2 >> ks3;
            ks.push_back(ks1);
            ks.push_back(ks2);
            ks.push_back(ks3);
        }
        else if (line.find("Ns") == 0)
        {
            iss >> readValue >> ns;
        }
    }

    mtlFile.close();
}

void Object3D::loadTexture()
{
    int width, height;

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

    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

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
        std::cout << "Failed to load texture " << texturePath << std::endl;
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Object3D::updateTrajectory()
{
    // Pegue os 4 pontos do segmento atual
    int idx = currentPointIndex * 3;
    if (idx + 3 >= trajectoryPoints.size())
    {
        // Voltar para o início da trajetória para loop contínuo
        currentPointIndex = 0;
        idx = 0;
    }

    glm::vec3 p0 = trajectoryPoints[idx];
    glm::vec3 p1 = trajectoryPoints[idx + 1];
    glm::vec3 p2 = trajectoryPoints[idx + 2];
    glm::vec3 p3 = trajectoryPoints[idx + 3];



    // Avança o parâmetro t pela velocidade
    trajectoryT += trajectorySpeed;

    if (trajectoryT > 1.0f)
    {
        // Passa para o próximo segmento
        trajectoryT = 0.0f;
        currentPointIndex++;
        if (currentPointIndex * 3 + 3 >= trajectoryPoints.size())
            currentPointIndex = 0; // loop
    }

    // Calcula a posição na curva usando Bézier cúbica
    position = bezierCurve(trajectoryT, p0, p1, p2, p3);

    // Atualiza a matriz modelo para a nova posição
    updateModelMatrix();
}

glm::vec3 Object3D::bezierCurve(float t, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3)
{
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 point = uuu * P0; // (1-t)^3 * P0
    point += 3 * uu * t * P1;   // 3(1-t)^2 * t * P1
    point += 3 * u * tt * P2;   // 3(1-t) * t^2 * P2
    point += ttt * P3;          // t^3 * P3

    return point;
}
