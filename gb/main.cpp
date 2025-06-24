// Standard Library
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cassert>
#include <cmath>

// OpenGL Loader
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project Headers
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "ObjectConfig.h"
#include "ObjectReader.h"

using namespace std;
using namespace glm;
namespace filesystem = std::filesystem;

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

// Protótipos das funções
void printCommands();
GLFWwindow *initializeGL();
void setupInitialObjects(vector<Object3D> &objects, Shader &shader);
void setupGeometry(Object3D *object);
void handleLighting(GLuint shaderID);
void transformObject(Object3D &object);
void drawObject(const Object3D &object, GLuint shaderID);
void handleTranslation(Object3D &object);
void handleRotation(Object3D &object);
void handleScale(Object3D &object);

bool incrementScale = false, decrementScale = false, isRotating = false, isTranslating = false;
bool actionW = false, actionS = false, actionA = false, actionD = false, actionI = false, actionJ = false;
Camera camera;
vector<Object3D> objects;

int main()
{
    printCommands();

    auto window = initializeGL();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    filesystem::path rootDir = filesystem::current_path();
    auto vertexShaderPath = (rootDir / "gb" / "shaders" / "vertex-shader.vs").string();
    auto fragmentShaderPath = (rootDir / "gb" / "shaders" / "fragment-shader.fs").string();
    Shader shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    glUseProgram(shader.ID);

    camera.initialize(&shader, width, height, 0.05f, 0.05f, -90.0f, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 15.0f));

    objects = loadObjectConfigs((rootDir / "gb" / "config" / "objects.json").string(), rootDir.string());

    setupInitialObjects(objects, shader);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.update();

        for (int i = 0; i < objects.size(); i++)
        {
            handleLighting(shader.ID);
            transformObject(objects[i]);
            drawObject(objects[i], shader.ID);
        }

        glfwSwapBuffers(window);
    }

    for (auto &obj : objects)
    {
        glDeleteVertexArrays(1, &obj.VAO);
    }

    glfwTerminate();
    return 0;
}

void printCommands()
{
    // TODO: Adicionar qual o comando vai startar e pausar o movimento com curva
    cout << "Comandos para interagir com a cena:" << endl;
    cout << "==========================================================" << endl;
    cout << "Seleção de objetos:" << endl;
    cout << "   ==> C - Selecionar câmera" << endl;
    cout << "   ==> 0 - Selecionar a pokebola" << endl;
    cout << "   ==> 1 - Selecionar a esfera minecraft" << endl;
    cout << "==========================================================" << endl;
    cout << "Quando a câmera estiver selecionada:" << endl;
    cout << "   ==> A, W, S, D - movimentam a câmera" << endl;
    cout << "   ==> a câmera também acompanha a movimentação do mouse" << endl;
    cout << "==========================================================" << endl;
    cout << "Quando um dos objetos estiver selecionado:" << endl;
    cout << "   ==> [ - para diminuit a escala" << endl;
    cout << "   ==> ] - para aumentar a escala" << endl;
    cout << "   ==> R - para rotacionar" << endl;
    cout << "       ==> W e S - para rotacionar no eixo X" << endl;
    cout << "       ==> A e D - para rotacionar no eixo Y" << endl;
    cout << "       ==> I e J - para rotacionar no eixo Z" << endl;
    cout << "   ==> T - para transladar" << endl;
    cout << "       ==> W e S - para transladar no eixo X" << endl;
    cout << "       ==> A e D - para transladar no eixo Y" << endl;
    cout << "       ==> I e J - para transladar no eixo Z" << endl;
}

void handleLighting(GLuint shaderID)
{
    glm::vec3 backLightPos = glm::vec3(-1.0f, 1.0f, -1.0f);
    glm::vec3 keyLightPos = glm::vec3(-1.0f, -1.0f, 1.0f);
    glm::vec3 fillLightPos = glm::vec3(1.0f, -1.0f, 1.0f);

    glUniform3fv(glGetUniformLocation(shaderID, "lightPosKey"), 1, glm::value_ptr(keyLightPos));
    glUniform3fv(glGetUniformLocation(shaderID, "lightPosFill"), 1, glm::value_ptr(fillLightPos));
    glUniform3fv(glGetUniformLocation(shaderID, "lightPosBack"), 1, glm::value_ptr(backLightPos));
}

void transformObject(Object3D &object)
{
    if (object.isSelected)
    {
        handleRotation(object);
        handleTranslation(object);
        handleScale(object);
    }
}

void handleRotation(Object3D &object)
{
    if (isRotating)
    {
        if (actionW)
        {
            object.model = glm::rotate(object.model, glm::radians(-0.9f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else if (actionS)
        {
            object.model = glm::rotate(object.model, glm::radians(0.9f), glm::vec3(-0.1f, 0.0f, 0.0f));
        }
        else if (actionA)
        {
            object.model = glm::rotate(object.model, glm::radians(0.9f), glm::vec3(0.0f, -0.1f, 0.0f));
        }
        else if (actionD)
        {
            object.model = glm::rotate(object.model, glm::radians(0.9f), glm::vec3(0.0f, 0.1f, 0.0f));
        }
        else if (actionJ)
        {
            object.model = rotate(object.model, radians(0.9f), vec3(0.0f, 0.0f, 0.1f));
        }
        else if (actionI)
        {
            object.model = rotate(object.model, radians(-0.9f), vec3(0.0f, 0.0f, 0.1f));
        }
    }
}

void handleTranslation(Object3D &object)
{
    if (isTranslating)
    {
        if (actionW)
        {
            object.model = glm::translate(object.model, glm::vec3(0.0f, 0.1f, 0.0f));
        }
        else if (actionS)
        {
            object.model = glm::translate(object.model, glm::vec3(0.0f, -0.1f, 0.0f));
        }
        else if (actionA)
        {
            object.model = glm::translate(object.model, glm::vec3(-0.1f, 0.0f, 0.0f));
        }
        else if (actionD)
        {
            object.model = glm::translate(object.model, glm::vec3(0.1f, 0.0f, 0.0f));
        }
        else if (actionJ)
        {
            object.model = glm::translate(object.model, glm::vec3(0.0f, 0.0f, 0.1f));
        }
        else if (actionI)
        {
            object.model = glm::translate(object.model, glm::vec3(0.0f, 0.0f, -0.1f));
        }
    }
}

void handleScale(Object3D &object)
{
    if (incrementScale)
    {
        incrementScale = false;
        object.model = glm::scale(object.model, glm::vec3(1.1f, 1.1f, 1.1f));
    }

    if (decrementScale)
    {
        decrementScale = false;
        object.model = glm::scale(object.model, glm::vec3(0.9f, 0.9f, 0.9f));
    }
}

void drawObject(const Object3D &object, GLuint shaderID)
{
    glBindVertexArray(object.VAO);
    glBindTexture(GL_TEXTURE_2D, object.texID);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(object.model));
    glDrawArrays(GL_TRIANGLES, 0, (object.positions.size() / 3));
    glBindVertexArray(0);
}

GLFWwindow *initializeGL()
{
    // Inicialização da GLFW
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    int screenWidth = mode->height;
    int screenHeight = mode->height;

    // Criação da janela GLFW
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "Bye CG!", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Fazendo o registro da função de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetCursorPos(window, screenWidth / 2, screenHeight / 2);

    // GLAD: carrega todos os ponteiros d funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    return window;
}

void setupInitialObjects(vector<Object3D> &objects, Shader &shader)
{
    for (int i = 0; i < objects.size(); i++)
    {
        loadOBJ(&objects[i]);
        loadMTL(&objects[i]);
        // Gerar o VAO para o objeto
        setupGeometry(&objects[i]);

        objects[i].texID = loadTexture(objects[i].texturePath);

        // Enviar a informação de qual variável armazenará o buffer da textura
        glUniform1i(glGetUniformLocation(shader.ID, "texBuff"), 0);

        glUniform1f(glGetUniformLocation(shader.ID, "ka"), objects[i].ka[0]);
        glUniform1f(glGetUniformLocation(shader.ID, "kd"), 0.5);
        glUniform1f(glGetUniformLocation(shader.ID, "ks"), objects[i].ks[0]);
        glUniform1f(glGetUniformLocation(shader.ID, "q"), objects[i].ns);

        // Ativando o primeiro buffer de textura da OpenGL
        glActiveTexture(GL_TEXTURE0);

        // Matriz de modelo: transformações na geometria (objeto)
        mat4 model = mat4(1.0f); // matriz identidade
        model = glm::translate(model, glm::vec3(objects[i].position.x, objects[i].position.y, objects[i].position.z));
        model = glm::rotate(model, glm::radians(objects[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(objects[i].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(objects[i].rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(objects[i].scale, objects[i].scale, objects[i].scale));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, value_ptr(model));
        objects[i].model = model;
    }
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        objects[0].isSelected = true;
        objects[1].isSelected = false;
        camera.isSelected = false;
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        objects[0].isSelected = false;
        objects[1].isSelected = true;
        camera.isSelected = false;
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        objects[0].isSelected = false;
        objects[1].isSelected = false;
        camera.isSelected = true;
    }

    if (camera.isSelected)
    {
        camera.move(window, key, action);
    }
    else
    {
        if (key == GLFW_KEY_R && action == GLFW_PRESS)
        {
            isRotating = true;
            isTranslating = false;
        }

        if (key == GLFW_KEY_T && action == GLFW_PRESS)
        {
            isTranslating = true;
            isRotating = false;
        }

        if (key == GLFW_KEY_W)
        {
            if (action == GLFW_PRESS)
            {
                actionW = true;
            }
            else if (action == GLFW_RELEASE)
                actionW = false;
        }

        if (key == GLFW_KEY_S)
        {
            if (action == GLFW_PRESS)
                actionS = true;
            else if (action == GLFW_RELEASE)
                actionS = false;
        }

        if (key == GLFW_KEY_A)
        {
            if (action == GLFW_PRESS)
                actionA = true;
            else if (action == GLFW_RELEASE)
                actionA = false;
        }

        if (key == GLFW_KEY_D)
        {
            if (action == GLFW_PRESS)
                actionD = true;
            else if (action == GLFW_RELEASE)
                actionD = false;
        }

        if (key == GLFW_KEY_I)
        {
            if (action == GLFW_PRESS)
                actionI = true;
            else if (action == GLFW_RELEASE)
                actionI = false;
        }

        if (key == GLFW_KEY_J)
        {
            if (action == GLFW_PRESS)
                actionJ = true;
            else if (action == GLFW_RELEASE)
                actionJ = false;
        }

        if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
        {
            decrementScale = true;
        }

        if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
        {
            incrementScale = true;
        }
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (camera.isSelected)
    {
        camera.rotate(window, xpos, ypos);
    }
}

void setupGeometry(Object3D *object)
{
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO[3];
    glGenBuffers(3, VBO);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, object->positions.size() * sizeof(GLfloat), object->positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    // Texture Coords
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, object->textureCoords.size() * sizeof(GLfloat), object->textureCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(1);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, object->normals.size() * sizeof(GLfloat), object->normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    object->VAO = VAO;
}
