#include <iostream>
#include <vector>
#include <filesystem>

// OpenGL Loader
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project Headers
#include "Shader.h"
#include "Camera.h"
#include "ObjectConfig.h"

using namespace std;
using namespace glm;
namespace filesystem = std::filesystem;

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

// Protótipos das funções
void printCommands();
GLFWwindow *initializeGL();
void handleLighting(GLuint shaderID);
void resetScaleVariables();

bool incrementScale = false, decrementScale = false, isRotating = false, isTranslating = false, isCurving = false;
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

    objects = loadObjectConfigs((rootDir / "gb" / "config" / "objects.json").string(), rootDir.string(), &shader);

    for (int i = 0; i < objects.size(); i++)
    {
        objects[i].initialSetup();
    }

    handleLighting(shader.ID);

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
            objects[i].transform(
                isRotating,
                isTranslating,
                isCurving,
                actionW,
                actionS,
                actionA,
                actionD,
                actionI,
                actionJ,
                incrementScale,
                decrementScale);

            objects[i].draw();
        }

        resetScaleVariables();
        glfwSwapBuffers(window);
    }

    for (auto &obj : objects)
    {
        glDeleteVertexArrays(1, &obj.VAO);
    }

    glfwTerminate();
    return 0;
}

void resetScaleVariables()
{
    incrementScale = false;
    decrementScale = false;
}

void printCommands()
{
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
    cout << "   ==> X - para ativar o movimento em curva" << endl;
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

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        objects[0].select();
        objects[1].unselect();
        camera.unselect();
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        objects[0].unselect();
        objects[1].select();
        camera.unselect();
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        objects[0].unselect();
        objects[1].unselect();
        camera.select();
    }

    if (camera.isSelected())
    {
        camera.move(window, key, action);
    }
    else
    {
        if (key == GLFW_KEY_R && action == GLFW_PRESS)
        {
            isRotating = true;
            isTranslating = false;
            isCurving = false;
        }

        if (key == GLFW_KEY_T && action == GLFW_PRESS)
        {
            isTranslating = true;
            isRotating = false;
            isCurving = false;
        }

        if (key == GLFW_KEY_X && action == GLFW_PRESS)
        {
            isCurving = true;
            isRotating = false;
            isTranslating = false;
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
    if (camera.isSelected())
    {
        camera.rotate(window, xpos, ypos);
    }
}
