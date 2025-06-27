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
    float trajectoryT;     // tempo de animação da trajetória
    float trajectorySpeed; // velocidade da animação da trajetória
    glm::vec3 backLightPos;
    glm::vec3 keyLightPos;
    glm::vec3 fillLightPos;

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
    int keyPressed = -1;
    bool isRotating = false;
    bool isTranslating = false;
    bool isCurving = false;
    bool incrementScale = false;
    bool decrementScale = false;

    void loadOBJ();
    void loadMTL();
    void loadTexture();
    void setupGeometry();
    void verifyScaleAction();
    void translate();
    void rotate();
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
        glm::vec3 backLightPos,
        glm::vec3 keyLightPos,
        glm::vec3 fillLightPos,
        Shader *shader);

    void initialSetup();
    void transform();
    void draw();
    void keyAction(int key, int action);
    void select() { isSelected = true; };
    void unselect() { isSelected = false; };
    void updateTrajectory();
    glm::vec3 bezierCurve(
        float t,
        glm::vec3 P0,
        glm::vec3 P1,
        glm::vec3 P2,
        glm::vec3 P3);
};