#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

struct Matrix
{
    float x, y, z;
};

class Object3D
{
private:
    // Propriedades do arquivo de configuração
    std::string name;
    std::string objPath;
    std::string mtlPath;
    std::string texturePath;
    float scale;
    Matrix position;
    Matrix rotation;

    // Propriedades do objeto 3D lidas do arquivo OBJ/MTL
    std::vector<GLfloat> positions;
    std::vector<GLfloat> textureCoords;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> ka;
    std::vector<GLfloat> ks;
    float ns;

    // Propriedades do objeto 3D definidos em runtime
    Shader *shader;
    GLuint texID;
    glm::mat4 model;
    bool isSelected;

    void loadOBJ();
    void loadMTL();
    void loadTexture();
    void setupGeometry();
    void translate(
        bool actionW,
        bool actionS,
        bool actionA,
        bool actionD,
        bool actionI,
        bool actionJ);
    void rotate(
        bool actionW,
        bool actionS,
        bool actionA,
        bool actionD,
        bool actionI,
        bool actionJ);

public:
    GLuint VAO;
    void initialize(
        std::string name,
        std::string objPath,
        std::string mtlPath,
        std::string texturePath,
        float scale,
        Matrix position,
        Matrix rotation,
        Shader *shader
    );

    void initialSetup();

    void transform(
        bool isRotating,
        bool isTranslating,
        bool actionW,
        bool actionS,
        bool actionA,
        bool actionD,
        bool actionI,
        bool actionJ,
        bool incrementScale,
        bool decrementScale);

    void draw();

    void select();
    void unselect();
};