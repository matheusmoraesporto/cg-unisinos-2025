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

using namespace std;
using namespace glm;
namespace filesystem = std::filesystem;

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

// Protótipos das funções
GLFWwindow *initializeGL();
int setupShader();
void setupGeometry(Object3D *object);
GLuint loadTexture(string filePath);
void loadOBJ(Object3D *object);
void loadMTL(Object3D *object);
void setupInitialObjects(vector<Object3D> &objects, Shader &shader);

bool incrementScale = false;
bool decrementScale = false;
bool rotateW = false, rotateS = false, rotateA = false, rotateD = false, rotateI = false, rotateJ = false;
Camera camera;
vector<Object3D> objects;

int main()
{
    auto window = initializeGL();
    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    filesystem::path rootDir = filesystem::current_path();
    auto vertexShaderPath = (rootDir / "gb" / "shaders" / "vertex-shader.vs").string();
    auto fragmentShaderPath = (rootDir / "gb" / "shaders" / "fragment-shader.fs").string();
    Shader shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    glUseProgram(shader.ID);

    camera.initialize(&shader, width, height, 0.05f, 0.05f, -90.0f, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.5f, 0.0f, 15.0f));

    objects = loadObjectConfigs((rootDir / "gb" / "config" / "objects.json").string(), rootDir.string());

    glm::vec3 backLightPos = glm::vec3(-1.0f, 1.0f, -1.0f);
    glm::vec3 keyLightPos = glm::vec3(-1.0f, -1.0f, 1.0f);
    glm::vec3 fillLightPos = glm::vec3(1.0f, -1.0f, 1.0f);

    setupInitialObjects(objects, shader);

    glEnable(GL_DEPTH_TEST);

    // Loop da aplicação - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
        glfwPollEvents();

        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.update();

        for (int i = 0; i < objects.size(); i++)
        {
            glUniform3fv(glGetUniformLocation(shader.ID, "lightPosKey"), 1, glm::value_ptr(keyLightPos));
            glUniform3fv(glGetUniformLocation(shader.ID, "lightPosFill"), 1, glm::value_ptr(fillLightPos));
            glUniform3fv(glGetUniformLocation(shader.ID, "lightPosBack"), 1, glm::value_ptr(backLightPos));

            if (objects[i].isSelected)
            {
                if (incrementScale)
                {
                    incrementScale = false;
                    objects[i].model = glm::scale(objects[i].model, glm::vec3(1.1f, 1.1f, 1.1f));
                }

                if (decrementScale)
                {
                    decrementScale = false;
                    objects[i].model = glm::scale(objects[i].model, glm::vec3(0.9f, 0.9f, 0.9f));
                }

                // Atualiza rotação acumulativa
                if (rotateW) // Rotação para cima (em torno de X)
                {
                    objects[i].model = glm::translate(objects[i].model, glm::vec3(0.0f, 0.01f, 0.0f));
                    // objects[i].model = rotate(objects[i].model, radians(1.0f), vec3(1.0f, 0.0f, 0.0f));
                }
                else if (rotateS) // Rotação para baixo (em torno de X)
                {
                    objects[i].model = rotate(objects[i].model, radians(-1.0f), vec3(1.0f, 0.0f, 0.0f));
                }
                else if (rotateA) // Rotação para a esquerda (em torno de Y)
                {
                    objects[i].model = rotate(objects[i].model, radians(1.0f), vec3(0.0f, 1.0f, 0.0f));
                }
                else if (rotateD) // Rotação para a direita (em torno de Y)
                {
                    objects[i].model = rotate(objects[i].model, radians(-1.0f), vec3(0.0f, 1.0f, 0.0f));
                }
                else if (rotateJ) // Rotação em torno de Z (sentido horário)
                {
                    objects[i].model = rotate(objects[i].model, radians(1.0f), vec3(0.0f, 0.0f, 1.0f));
                }
                else if (rotateI) // Rotação em torno de Z (sentido anti-horário)
                {
                    objects[i].model = rotate(objects[i].model, radians(-1.0f), vec3(0.0f, 0.0f, 1.0f));
                }
            }

            glBindVertexArray(objects[i].VAO);              // Conectando ao buffer de geometria
            glBindTexture(GL_TEXTURE_2D, objects[i].texID); // conectando com o buffer de textura que será usado no draw

            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, value_ptr(objects[i].model));

            // glUniform4f(glGetUniformLocation(shaderID, "inputColor"), color.r, color.g, color.b, 1.0f); // enviando cor para variável uniform inputColor
            //   Chamada de desenho - drawcall
            //   Poligono Preenchido - GL_TRIANGLES
            glDrawArrays(GL_TRIANGLES, 0, (objects[i].positions.size() / 3));

            glBindVertexArray(0); // Desconectando o buffer de geometria
        }

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }
    // Pede pra OpenGL desalocar os buffers

    for (auto &obj : objects)
    {
        glDeleteVertexArrays(1, &obj.VAO);
    }
    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
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

        cout << "VAO ID: " << objects[i].VAO << endl;
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
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, value_ptr(model));
        model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
        model = glm::translate(model, glm::vec3(objects[i].initialPosition.x, objects[i].initialPosition.y, objects[i].initialPosition.z));
        // model = glm::rotate(model, radians(1500.0f), glm::vec3(0.0f, 12.5f, 0.0f));
        objects[i].model = model;
    }
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
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
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (key == GLFW_KEY_W)
        {
            if (action == GLFW_PRESS)
            {
                rotateW = true;
            }
            else if (action == GLFW_RELEASE)
                rotateW = false;
        }

        if (key == GLFW_KEY_S)
        {
            if (action == GLFW_PRESS)
                rotateS = true;
            else if (action == GLFW_RELEASE)
                rotateS = false;
        }

        if (key == GLFW_KEY_A)
        {
            if (action == GLFW_PRESS)
                rotateA = true;
            else if (action == GLFW_RELEASE)
                rotateA = false;
        }

        if (key == GLFW_KEY_D)
        {
            if (action == GLFW_PRESS)
                rotateD = true;
            else if (action == GLFW_RELEASE)
                rotateD = false;
        }

        if (key == GLFW_KEY_I)
        {
            if (action == GLFW_PRESS)
                rotateI = true;
            else if (action == GLFW_RELEASE)
                rotateI = false;
        }

        if (key == GLFW_KEY_J)
        {
            if (action == GLFW_PRESS)
                rotateJ = true;
            else if (action == GLFW_RELEASE)
                rotateJ = false;
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
    GLuint VAO, VBO[3];

    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, object->positions.size() * sizeof(GLfloat), object->positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, object->textureCoords.size() * sizeof(GLfloat), object->textureCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, object->normals.size() * sizeof(GLfloat), object->normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    object->VAO = VAO;
}

GLuint loadTexture(string filePath)
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

void loadOBJ(Object3D *object)
{
    vector<glm::vec3> vertexIndices;
    vector<glm::vec2> textureIndices;
    vector<glm::vec3> normalIndices;

    ifstream file(object->objPath);

    if (!file.is_open())
    {
        cerr << "Failed to open file: " << object->objPath << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        istringstream iss(line);
        string prefix;
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
            string v1, v2, v3;
            iss >> v1 >> v2 >> v3;

            glm::ivec3 vIndices, tIndices, nIndices;
            istringstream(v1.substr(0, v1.find('/'))) >> vIndices.x;
            istringstream(v1.substr(v1.find('/') + 1, v1.rfind('/') - v1.find('/') - 1)) >> tIndices.x;
            istringstream(v1.substr(v1.rfind('/') + 1)) >> nIndices.x;
            istringstream(v2.substr(0, v2.find('/'))) >> vIndices.y;
            istringstream(v2.substr(v2.find('/') + 1, v2.rfind('/') - v2.find('/') - 1)) >> tIndices.y;
            istringstream(v2.substr(v2.rfind('/') + 1)) >> nIndices.y;
            istringstream(v3.substr(0, v3.find('/'))) >> vIndices.z;
            istringstream(v3.substr(v3.find('/') + 1, v3.rfind('/') - v3.find('/') - 1)) >> tIndices.z;
            istringstream(v3.substr(v3.rfind('/') + 1)) >> nIndices.z;

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
    string line, readValue;
    ifstream mtlFile(object->mtlPath);

    cout << "Loading MTL file: " << object->mtlPath << endl;

    if (!mtlFile.is_open())
    {
        cerr << "Failed to open file: " << object->mtlPath << endl;
        return;
    }

    while (getline(mtlFile, line))
    {
        istringstream iss(line);

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
