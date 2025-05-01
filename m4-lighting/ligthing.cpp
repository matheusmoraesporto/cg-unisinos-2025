/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 12/05/2023
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <optional>
#include <vector>
#include <filesystem>
#include <map>
 
using namespace std;
 
// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include "stb_image.h"
 
// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int setupGeometry();
int loadTexture(string path);
void loadOBJ(string path);
void loadMTL(string path);

GLFWwindow* initializeGL();
int setupShader();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;
float scale = 1.0f;
bool needScale = false;
bool incrementScale = false;
bool decrementScale = false;
bool rotateW=false, rotateS=false, rotateA=false, rotateD=false, rotateI=false, rotateJ=false;
string objPath = "/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/objects/suzanneTri.obj";
string mtlPath = "/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/objects/";
string texturePath = "/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/objects/";
string mtlFile = "";
string textureFile = "";
vector<GLfloat> positions;
vector<GLfloat> textureCoords;
vector<GLfloat> normals;
vector<GLfloat> ka;
vector<GLfloat> ks;
float ns;

// Função MAIN
int main()
{
    GLFWwindow* window = initializeGL();

    Shader shader("/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/shaders/vertex-shader.vs", "/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/shaders/fragment-shader.fs");
    loadOBJ(objPath);
    loadMTL(mtlPath + mtlFile);
    
    GLuint textureID = loadTexture(texturePath + textureFile);
    GLuint VAO = setupGeometry();

    glUseProgram(shader.ID);

    glm::mat4 model = glm::mat4(1); 
    GLint modelLoc = glGetUniformLocation(shader.ID, "model");

    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.7f));
    glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (incrementScale) {
            model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
            incrementScale = false;
        }

        if (decrementScale) {
            model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
            decrementScale = false;
        }

        float angleX=0.1f, angleY=0.1f, angleZ=0.1f;
        if (rotateW)
        {
            model = glm::rotate(model, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else if (rotateS) 
        {
            model = glm::rotate(model, angleX, glm::vec3(-1.0f, 0.0f, 0.0f));
        }
        else if (rotateI)
        {
            model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if (rotateJ)
        {
            model = glm::rotate(model, angleY, glm::vec3(0.0f, -1.0f, 0.0f));
        }
        else if (rotateA)
        {
            model = glm::rotate(model, angleZ, glm::vec3(0.0f, 0.0f, 1.0f));
        }
        else if (rotateD)
        {
            model = glm::rotate(model, angleZ, glm::vec3(0.0f, 0.0f, -1.0f));
        }

        glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, (positions.size() / 3));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

// Função de callback de teclado - só pode ter uma instäncia (deve ser estática se
// estiver dentro de uma classe) - é chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS)
            rotateW = true;
        else if (action == GLFW_RELEASE)
            rotateW = false;
    }

    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS)
            rotateS = true;
        else if (action == GLFW_RELEASE)
            rotateS = false;
    }

    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS)
            rotateA = true;
        else if (action == GLFW_RELEASE)
            rotateA = false;
    }

    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS)
            rotateD = true;
        else if (action == GLFW_RELEASE)
            rotateD = false;
    }

    if (key == GLFW_KEY_I) {
        if (action == GLFW_PRESS)
            rotateI = true;
        else if (action == GLFW_RELEASE)
            rotateI = false;
    }

    if (key == GLFW_KEY_J) {
        if (action == GLFW_PRESS)
            rotateJ = true;
        else if (action == GLFW_RELEASE)
            rotateJ = false;
    }

    if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
        needScale = true;
        decrementScale = true;
    }

    if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
        needScale = true;
        incrementScale = true;
    }
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
 {
     GLuint VAO, VBO[3];
 
     glGenVertexArrays(1, &VAO);
     glGenBuffers(3, VBO);
 
     glBindVertexArray(VAO);
 
     glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
     glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), positions.data(), GL_STATIC_DRAW);
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
     glEnableVertexAttribArray(0);
 
     glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
     glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(GLfloat), textureCoords.data(), GL_STATIC_DRAW);
     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
     glEnableVertexAttribArray(1);
 
     glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
     glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
     glEnableVertexAttribArray(2);
 
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindVertexArray(0);
 
     glEnable(GL_DEPTH_TEST);
 
     return VAO;
 
 }

GLFWwindow* initializeGL()
{
    // Inicialização da GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Configuração do contexto OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Olá 3D - Matheus Porto!", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // Inicialização do GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    return window;
}
 
void loadOBJ(string path)
 {
     vector<glm::vec3> vertexIndices;
     vector<glm::vec2> textureIndices;
     vector<glm::vec3> normalIndices;
 
     ifstream file(path);
     if (!file.is_open())
     {
         cerr << "Failed to open file: " << path << endl;
         return;
     }
 
     string line;
     while (getline(file, line))
     {
         istringstream iss(line);
         string prefix;
         iss >> prefix;
 
         if (prefix == "mtllib")
		 {
			 iss >> mtlFile;
		 }
         else if (prefix == "v")
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
                 const glm::vec3& vertex = vertexIndices[vIndices[i] - 1];
                 const glm::vec2& texture = textureIndices[tIndices[i] - 1];
                 const glm::vec3& normal = normalIndices[nIndices[i] - 1];
 
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

 void loadMTL(string path)
 {
     string line, readValue;
     ifstream mtlFile(path);
 
     if (!mtlFile.is_open())
     {
         cerr << "Failed to open file: " << path << endl;
         return;
     }

     while (getline(mtlFile, line))
     {
         istringstream iss(line);
 
         if (line.find("map_Kd") == 0)
		 {
		 	 iss >> readValue >> textureFile;
		 }
         else if (line.find("Ka") == 0)
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
 
 int loadTexture(string path)
 {
     stbi_set_flip_vertically_on_load(true);

     GLuint texID;
 
     glGenTextures(1, &texID);
     glBindTexture(GL_TEXTURE_2D, texID);
 
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
 
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
     int width, height, nrChannels;
     unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
 
     if (data)
     {
         if (nrChannels == 3)
         {
             glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
         }
         else
         {
             glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
         }
         glGenerateMipmap(GL_TEXTURE_2D);
     }
     else
     {
         cout << "Failed to load texture" << endl;
     }
     stbi_image_free(data);
     glBindTexture(GL_TEXTURE_2D, 0);
     return texID;
 }
