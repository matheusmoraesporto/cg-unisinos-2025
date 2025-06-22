/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 13/08/2024
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

using namespace glm;

#include <cmath>

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

// Protótipos das funções
int setupShader();
GLuint loadTexture(string filePath, int &width, int &height);
void loadOBJ(string path);
void loadMTL(string path);
void toggleLight(int shaderID, string light, vec3 vec);
void drawGeometry(GLuint shaderID, GLuint VAO, vec3 position, vec3 dimensions, float angle, int nVertices, vec3 color = vec3(1.0, 0.0, 0.0), vec3 axis = (vec3(0.0, 0.0, 1.0)));
GLuint generateSphere(float radius, int latSegments, int lonSegments, int &nVertices);
void handleLighting(GLuint shaderID);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 800;
bool incrementScale = false;
bool decrementScale = false;
bool actionW = false, actionS = false, actionA = false, actionD = false, actionI = false, actionJ = false;
bool isRotation = false;
bool isTranslation = false;
string vertexShaderPath = "/Users/i559431/unisinos/cg-unisinos-2025/m5-vivencial/shaders/vertex-shader.vs";
string fragmentShaderPath = "/Users/i559431/unisinos/cg-unisinos-2025/m5-camera/shaders/fragment-shader.fs";
string objPath = "/Users/i559431/unisinos/cg-unisinos-2025/m5-camera/objects/sphere.obj";
string mtlPath = "/Users/i559431/unisinos/cg-unisinos-2025/m5-camera/objects/";
string texturePath = "/Users/i559431/unisinos/cg-unisinos-2025/m5-camera/objects/minecraft.jpg";
string mtlFile = "";
string textureFile = "";
vector<GLfloat> positions;
vector<GLfloat> textureCoords;
vector<GLfloat> normals;
vector<GLfloat> ka;
vector<GLfloat> ks;
float ns;
Camera camera;

// Função MAIN
int main()
{
    // Inicialização da GLFW
    glfwInit();

    // Muita atenção aqui: alguns ambientes não aceitam essas configurações
    // Você deve adaptar para a versão do OpenGL suportada por sua placa
    // Sugestão: comente essas linhas de código para desobrir a versão e
    // depois atualize (por exemplo: 4.5 com 4 e 5)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// Essencial para computadores da Apple
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Criação da janela GLFW
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Olá esfera iluminada!", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Fazendo o registro da função de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // GLAD: carrega todos os ponteiros d funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compilando e buildando o programa de shader
    Shader shader("/Users/i559431/unisinos/cg-unisinos-2025/m5-camera/shaders/vertex-shader.vs", "/Users/i559431/unisinos/cg-unisinos-2025/m5-camera/shaders/fragment-shader.fs");
    loadOBJ(objPath);
    loadMTL(mtlPath + mtlFile);

    // Gerando um buffer simples, com a geometria de um triângulo
    int nVertices;
    GLuint VAO = generateSphere(0.5, 16, 16, nVertices);

    // Carregando uma textura e armazenando seu id
    int imgWidth, imgHeight;
    GLuint texID = loadTexture(texturePath, imgWidth, imgHeight);

    glUseProgram(shader.ID);

    camera.initialize(&shader, width, height);
    Mesh mineSphere;
    mineSphere.initialize(VAO, positions.size() / 3, &shader, texID);

    // Enviar a informação de qual variável armazenará o buffer da textura
    glUniform1i(glGetUniformLocation(shader.ID, "texBuff"), 0);

    glUniform1f(glGetUniformLocation(shader.ID, "ka"), ka[0]);
    glUniform1f(glGetUniformLocation(shader.ID, "kd"), 0.5);
    glUniform1f(glGetUniformLocation(shader.ID, "ks"), ks[0]);
    glUniform1f(glGetUniformLocation(shader.ID, "q"), ns);

    // glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camPos.x, camPos.y, camPos.z);

    // Ativando o primeiro buffer de textura da OpenGL
    glActiveTexture(GL_TEXTURE0);

    // Matriz de projeção paralela ortográfica
    // mat4 projection = ortho(-10.0, 10.0, -10.0, 10.0, -1.0, 1.0);
    // mat4 projection = ortho(-1.0, 1.0, -1.0, 1.0, -3.0, 3.0);
    // glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, value_ptr(projection));

    // Matriz de modelo: transformações na geometria (objeto)
    mat4 model = mat4(1); // matriz identidade
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, value_ptr(model));

    glEnable(GL_DEPTH_TEST);

    // Loop da aplicação - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
        glfwPollEvents();

        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        handleLighting(shader.ID);

        camera.update();
        mineSphere.draw();

        if (incrementScale)
        {
            incrementScale = false;
            model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
        }

        if (decrementScale)
        {
            decrementScale = false;
            model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
        }

        if (isRotation)
        {
            // Atualiza rotação acumulativa
            if (actionW) // Rotação para cima (em torno de X)
            {
                model = rotate(model, radians(1.0f), vec3(1.0f, 0.0f, 0.0f));
            }
            else if (actionS) // Rotação para baixo (em torno de X)
            {
                model = rotate(model, radians(-1.0f), vec3(1.0f, 0.0f, 0.0f));
            }
            else if (actionA) // Rotação para a esquerda (em torno de Y)
            {
                model = rotate(model, radians(1.0f), vec3(0.0f, 1.0f, 0.0f));
            }
            else if (actionD) // Rotação para a direita (em torno de Y)
            {
                model = rotate(model, radians(-1.0f), vec3(0.0f, 1.0f, 0.0f));
            }
            else if (actionJ) // Rotação em torno de Z (sentido horário)
            {
                model = rotate(model, radians(1.0f), vec3(0.0f, 0.0f, 1.0f));
            }
            else if (actionI) // Rotação em torno de Z (sentido anti-horário)
            {
                model = rotate(model, radians(-1.0f), vec3(0.0f, 0.0f, 1.0f));
            }
        }

        if (isTranslation)
        {
            if (actionW)
            {
                model = glm::translate(model, glm::vec3(0.0f, 0.01f, 0.0f));
            }
            else if (actionS)
            {
                model = glm::translate(model, glm::vec3(0.0f, -0.01f, 0.0f));
            }
            else if (actionA)
            {
                model = glm::translate(model, glm::vec3(-0.01f, 0.0f, 0.0f));
            }
            else if (actionD)
            {
                model = glm::translate(model, glm::vec3(0.01f, 0.0f, 0.0f));
            }
        }

        mineSphere.update(model);

        glBindVertexArray(VAO);              // Conectando ao buffer de geometria
        glBindTexture(GL_TEXTURE_2D, texID); // conectando com o buffer de textura que será usado no draw

        // glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, value_ptr(model));

        // glUniform4f(glGetUniformLocation(shaderID, "inputColor"), color.r, color.g, color.b, 1.0f); // enviando cor para variável uniform inputColor
        //   Chamada de desenho - drawcall
        //   Poligono Preenchido - GL_TRIANGLES
        // glDrawArrays(GL_TRIANGLES, 0, (positions.size() / 3));

        // glBindVertexArray(0); // Desconectando o buffer de geometria

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }
    // Pede pra OpenGL desalocar os buffers
    glDeleteVertexArrays(1, &VAO);
    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
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

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        // TODO: Uncomment me
        // objects[0].isSelected = false;
        // objects[1].isSelected = false;
        // camera.isSelected = true;

        // TODO: Remove me
        camera.isSelected = !camera.isSelected;
    }

    if (camera.isSelected)
        camera.move(window, key, action);
    else
    {
        if (key == GLFW_KEY_R && action == GLFW_PRESS)
        {
            isTranslation = false;
            isRotation = true;
        }

        if (key == GLFW_KEY_T && action == GLFW_PRESS)
        {
            isRotation = false;
            isTranslation = true;
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

void mouse_callback(GLFWwindow *window, double posX, double posY)
{
    if (camera.isSelected)
    {
        camera.rotate(window, posX, posY);
    }
}

GLuint loadTexture(string filePath, int &width, int &height)
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

void drawGeometry(GLuint shaderID, GLuint VAO, vec3 position, vec3 dimensions, float angle, int nVertices, vec3 color, vec3 axis)
{
    // Matriz de modelo: transformações na geometria (objeto)
    mat4 model = mat4(1); // matriz identidade
    // Translação
    model = translate(model, position);
    // Rotação
    model = rotate(model, radians(angle), axis);
    // Escala
    model = scale(model, dimensions);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

    // glUniform4f(glGetUniformLocation(shaderID, "inputColor"), color.r, color.g, color.b, 1.0f); // enviando cor para variável uniform inputColor
    //   Chamada de desenho - drawcall
    //   Poligono Preenchido - GL_TRIANGLES
    glDrawArrays(GL_TRIANGLES, 0, (positions.size() / 3));
}

GLuint generateSphere(float radius, int latSegments, int lonSegments, int &nVertices)
{
    GLuint VAO;
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

    return VAO;
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
