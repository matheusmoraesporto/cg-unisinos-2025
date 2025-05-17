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

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include "stb_image.h"
struct Vertex
{
    glm::vec3 position;
    glm::vec2 texcoord;
};

struct Obj
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::u32vec3> triangle_indices;
    std::vector<Vertex> vertices;
    std::vector<GLfloat> ka;
    std::vector<GLfloat> ks;
    float ns;
    std::string texture_path;
    std::string mtl_path;
    std::string obj_path;
    GLuint texID;
    GLuint VAO;
    glm::mat4 model;
    GLint modelLoc;
};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
GLFWwindow *initializeGL();
Obj loadOBJ(const std::string &filename);
void loadMTL(Obj *obj, int i);
void loadTexture(Obj *obj);
void setupGeometry(Obj *obj);
int setupShader();
void setupObjects(GLuint shaderID);
void handleSelectedObject();
void resetScaleVariables();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;
float scale = 1.0f;
bool needScale = false;
bool incrementScale = false;
bool decrementScale = false;
bool rotateW = false, rotateS = false, rotateA = false, rotateD = false, rotateI = false, rotateJ = false;
int selectedObject = 0;
const int objectsAmount = 2;
Obj objects[objectsAmount];
string objPath = "/Users/i559431/unisinos/cg-unisinos-2025/m2-vivencial/objects/";

// Função MAIN
int main()
{
    GLFWwindow *window = initializeGL();

    Shader shader(
        "/Users/i559431/unisinos/cg-unisinos-2025/m2-vivencial/shaders/vertex-shader.vs",
        "/Users/i559431/unisinos/cg-unisinos-2025/m2-vivencial/shaders/fragment-shader.fs");

    glUseProgram(shader.ID);
    setupObjects(shader.ID);

    glEnable(GL_DEPTH_TEST);

    cout << "First character selected" << endl;
    cout << "Press \"N\" to select next" << endl;

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < objectsAmount; i++)
        {
            handleSelectedObject();

            glUniformMatrix4fv(objects[i].modelLoc, 1, false, glm::value_ptr(objects[i].model));
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, objects[i].texID); // conectando com o buffer de textura que será usado no draw
            glBindVertexArray(objects[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, objects[i].vertices.size());
        }

        resetScaleVariables();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &objects[0].VAO);
    glfwTerminate();
    return 0;
}

void handleSelectedObject()
{
    if (incrementScale)
    {
        objects[selectedObject].model =
            glm::scale(objects[selectedObject].model, glm::vec3(1.1f, 1.1f, 1.1f));
    }

    if (decrementScale)
    {
        objects[selectedObject].model =
            glm::scale(objects[selectedObject].model, glm::vec3(0.9f, 0.9f, 0.9f));
    }

    if (rotateW)
    {
        objects[selectedObject].model =
            glm::rotate(objects[selectedObject].model, 0.01f, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else if (rotateS)
    {
        objects[selectedObject].model =
            glm::rotate(objects[selectedObject].model, 0.01f, glm::vec3(-1.0f, 0.0f, 0.0f));
    }
    else if (rotateI)
    {
        objects[selectedObject].model =
            glm::rotate(objects[selectedObject].model, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (rotateJ)
    {
        objects[selectedObject].model =
            glm::rotate(objects[selectedObject].model, 0.01f, glm::vec3(0.0f, -1.0f, 0.0f));
    }
    else if (rotateA)
    {
        objects[selectedObject].model =
            glm::rotate(objects[selectedObject].model, 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    else if (rotateD)
    {
        objects[selectedObject].model =
            glm::rotate(objects[selectedObject].model, 0.01f, glm::vec3(0.0f, 0.0f, -1.0f));
    }
}

void resetScaleVariables()
{
    incrementScale = false;
    decrementScale = false;
}

// Função de callback de teclado - só pode ter uma instäncia (deve ser estática se
// estiver dentro de uma classe) - é chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_W)
    {
        if (action == GLFW_PRESS)
            rotateW = true;
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
        needScale = true;
        decrementScale = true;
    }

    if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
    {
        needScale = true;
        incrementScale = true;
    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        selectedObject = (selectedObject + 1) % 2;

        if (selectedObject == 0)
            cout << "You selected the first character" << endl;
        else
            cout << "You selected the second character" << endl;
    }
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
void setupGeometry(Obj *obj)
{
    GLuint VBO, VAO;

    // Geração do identificador do VBO
    glGenBuffers(1, &VBO);

    // Geração do identificador do EBO
    // glGenBuffers(1, &EBO);

    // Faz a conexão (vincula) do buffer como um buffer de array
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Envia os dados do array de floats para o buffer da OpenGl
    size_t vertex_stride_size = sizeof(Vertex);
    size_t vertex_total_size = obj->vertices.size() * vertex_stride_size;
    glBufferData(GL_ARRAY_BUFFER, vertex_total_size, obj->vertices.data(), GL_STATIC_DRAW);

    // Geração do identificador do VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);

    // Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
    // e os ponteiros para os atributos
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_stride_size, (GLvoid *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_stride_size, (GLvoid *)offsetof(Vertex, texcoord));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    obj->VAO = VAO;
}

void setupObjects(GLuint shaderID)
{
    for (int i = 0; i < objectsAmount; i++)
    {
        objects[i] = loadOBJ("/Users/i559431/unisinos/cg-unisinos-2025/m2-vivencial/objects/suzanneTri.obj");
        loadMTL(&objects[i], i);
        loadTexture(&objects[i]);
        setupGeometry(&objects[i]);

        auto model = glm::mat4(1);
        auto modelLoc = glGetUniformLocation(shaderID, "model");

        if (i == 0)
        {
            model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f));
        }
        else if (i == 1)
        {
            model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
        }

        model = glm::scale(model, glm::vec3(0.3f));
        glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));

        objects[i].model = model;
        objects[i].modelLoc = modelLoc;
    }
}

GLFWwindow *initializeGL()
{
    // Inicialização da GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Configuração do contexto OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criação da janela GLFW
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Olá 3D - Matheus Porto!", nullptr, nullptr);
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

Obj loadOBJ(const std::string &filename)
{
    auto file = std::ifstream(filename);

    Obj obj;
    char buf[BUFSIZ];
    while (file.getline(buf, sizeof(buf)))
    {
        std::stringstream ss(buf);
        std::string code;
        ss >> code;
        if (code == "v")
        {
            glm::vec3 v;
            ss >> v.x;
            ss >> v.y;
            ss >> v.z;
            obj.positions.push_back(v);
        }
        else if (code == "vn")
        {
            glm::vec3 vn;
            ss >> vn.x >> vn.y >> vn.z;
            obj.normals.push_back(vn);
        }
        else if (code == "vt")
        {
            glm::vec2 vt;
            ss >> vt.x;
            ss >> vt.y;
            obj.texcoords.push_back(vt);
        }
        else if (code == "f")
        {
            glm::u32vec3 fv;  // positions
            glm::u32vec3 fvt; // textcoord
            glm::u32vec3 fvn; // normal
            ss >> fv[0];
            ss.get() /*slash*/;
            ss >> fvt[0];
            ss.get() /*slash*/;
            ss >> fvn[0];
            ss >> fv[1];
            ss.get() /*slash*/;
            ss >> fvt[1];
            ss.get() /*slash*/;
            ss >> fvn[1];
            ss >> fv[2];
            ss.get() /*slash*/;
            ss >> fvt[2];
            ss.get() /*slash*/;
            ss >> fvn[2];
            fv -= glm::u32vec3(1);  /* index is offset by 1 */
            fvt -= glm::u32vec3(1); /* index is offset by 1 */
            fvn -= glm::u32vec3(1); /* index is offset by 1 */
            obj.triangle_indices.push_back(fv);
            Vertex v0;
            v0.position = obj.positions[fv[0]];
            v0.texcoord = obj.texcoords[fvt[0]];
            obj.vertices.push_back(v0);
            Vertex v1;
            v1.position = obj.positions[fv[1]];
            v1.texcoord = obj.texcoords[fvt[1]];
            obj.vertices.push_back(v1);
            Vertex v2;
            v2.position = obj.positions[fv[2]];
            v2.texcoord = obj.texcoords[fvt[2]];
            obj.vertices.push_back(v2);
        }
        else if (code == "mtllib")
        {
            std::string mtllib_str;
            ss >> mtllib_str;
            obj.mtl_path = objPath + mtllib_str;
        }
    }

    return obj;
}

void loadTexture(Obj *obj)
{
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
    int nrChannels, width, height;
    unsigned char *data = stbi_load(obj->texture_path.c_str(), &width, &height, &nrChannels, 0);

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
        std::cout << "Failed to load texture " << obj->texture_path << std::endl;
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    obj->texID = texID;
}

void loadMTL(Obj *obj, int i)
{
    string line, readValue;
    ifstream mtlFile(obj->mtl_path);

    if (!mtlFile.is_open())
    {
        cerr << "MTL: Failed to open file: " << obj->mtl_path << endl;
        return;
    }

    while (getline(mtlFile, line))
    {
        istringstream iss(line);

        if (line.find("map_Kd") == 0)
        {
            std::string texture;
            iss >> readValue >> texture;
            if (i == 0)
                obj->texture_path = objPath + "Suzanne.png";
            else
                obj->texture_path = objPath + "Suzanne-ugly.png";
        }
        else if (line.find("Ka") == 0)
        {
            float ka1, ka2, ka3;
            iss >> readValue >> ka1 >> ka2 >> ka3;
            obj->ka.push_back(ka1);
            obj->ka.push_back(ka2);
            obj->ka.push_back(ka3);
        }
        else if (line.find("Ks") == 0)
        {
            float ks1, ks2, ks3;
            iss >> readValue >> ks1 >> ks2 >> ks3;
            obj->ks.push_back(ks1);
            obj->ks.push_back(ks2);
            obj->ks.push_back(ks3);
        }
        else if (line.find("Ns") == 0)
        {
            iss >> readValue >> obj->ns;
        }
    }

    mtlFile.close();
}
