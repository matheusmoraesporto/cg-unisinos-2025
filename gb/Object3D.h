#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Object3D
{
private:
    // Propriedades do arquivo de configuração
    std::string name;
    std::string objPath;
    std::string mtlPath;
    std::string texturePath;
    float scale;
    glm::vec3 position;
    glm::vec3 rotation;
    std::vector<glm::vec3> trajectoryPoints;
    float trajectoryT; // tempo de animação da trajetória
    float trajectorySpeed; // velocidade da animação da trajetória

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
    int currentPointIndex = 0;
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
    void updateModelMatrix();

public:
    GLuint VAO;
    void initialize(
        std::string name,
        std::string objPath,
        std::string mtlPath,
        std::string texturePath,
        float scale,
        glm::vec3 position,
        glm::vec3 rotation,
        std::vector<glm::vec3> trajectoryPoints,
        float trajectorySpeed,
        Shader *shader
    );

    void initialSetup();

    void transform(
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
        bool decrementScale
    );

    void draw();

    void select();
    void unselect();
    void updateTrajectory();
    glm::vec3 bezierCurve(
        float t, 
        glm::vec3 P0, 
        glm::vec3 P1, 
        glm::vec3 P2, 
        glm::vec3 P3
    );
};