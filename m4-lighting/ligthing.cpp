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
 
 using namespace glm;
 
 #include <cmath>
 
 // Protótipo da função de callback de teclado
 void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
 
 // Protótipos das funções
 int setupShader();
 int setupGeometry();
 GLuint loadTexture(string filePath, int &width, int &height);
 void loadOBJ(string path);
 void loadMTL(string path);
 
 void drawGeometry(GLuint shaderID, GLuint VAO, vec3 position, vec3 dimensions, float angle, int nVertices, vec3 color= vec3(1.0,0.0,0.0), vec3 axis = (vec3(0.0, 0.0, 1.0)));
 GLuint generateSphere(float radius, int latSegments, int lonSegments, int &nVertices);
  
 // Dimensões da janela (pode ser alterado em tempo de execução)
 const GLuint WIDTH = 800, HEIGHT = 800;
 bool incrementScale = false;
 bool decrementScale = false;
 bool rotateW=false, rotateS=false, rotateA=false, rotateD=false, rotateI=false, rotateJ=false;
 string vertexShaderPath = "/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/shaders/vertex-shader.vs";
 string fragmentShaderPath = "/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/shaders/fragment-shader.fs";
 string objPath = "/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/objects/sphere.obj";
 string mtlPath = "/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/objects/";
 string texturePath = "/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/objects/minecraft.jpg";
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
     GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Ola esfera iluminada!", nullptr, nullptr);
     glfwMakeContextCurrent(window);
 
     // Fazendo o registro da função de callback para a janela GLFW
     glfwSetKeyCallback(window, key_callback);
 
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
     Shader shader("/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/shaders/vertex-shader.vs", "/Users/i559431/unisinos/cg-unisinos-2025/m4-lighting/shaders/fragment-shader.fs");
     loadOBJ(objPath);
     loadMTL(mtlPath + mtlFile);

     // Gerando um buffer simples, com a geometria de um triângulo
     int nVertices;
     GLuint VAO = generateSphere(0.5, 16, 16, nVertices);
 
     // Carregando uma textura e armazenando seu id
     int imgWidth, imgHeight;
     GLuint texID = loadTexture(texturePath, imgWidth, imgHeight);
 
    //  float ka = 0.1, kd =0.5, ks = 0.5, q = 10.0;
     vec3 lightPos = vec3(0.6, 1.2, -0.5);
     vec3 camPos = vec3(0.0,0.0,-3.0);
 
 
     glUseProgram(shader.ID);
 
     // Enviar a informação de qual variável armazenará o buffer da textura
     glUniform1i(glGetUniformLocation(shader.ID, "texBuff"), 0);
 
     glUniform1f(glGetUniformLocation(shader.ID, "ka"), ka[0]);
     glUniform1f(glGetUniformLocation(shader.ID, "kd"), 0.5);
     glUniform1f(glGetUniformLocation(shader.ID, "ks"), ks[0]);
     glUniform1f(glGetUniformLocation(shader.ID, "q"), ns);
     glUniform3f(glGetUniformLocation(shader.ID, "lightPos"), lightPos.x,lightPos.y,lightPos.z);
     glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camPos.x,camPos.y,camPos.z);
 
     //Ativando o primeiro buffer de textura da OpenGL
     glActiveTexture(GL_TEXTURE0);
     
 
     // Matriz de projeção paralela ortográfica
     // mat4 projection = ortho(-10.0, 10.0, -10.0, 10.0, -1.0, 1.0);
     mat4 projection = ortho(-1.0, 1.0, -1.0, 1.0, -3.0, 3.0);
     glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, value_ptr(projection));
 
     // Matriz de modelo: transformações na geometria (objeto)
     mat4 model = mat4(1); // matriz identidade
     glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, value_ptr(model));
    
     vec3 pos = vec3(0.0, 0.0, 0.0);
     vec3 dimensions = vec3(1, 1, 1);
     vec3 axis = vec3(1.0f, 1.0f, 1.0f); // Eixo inicial (Y)
     float angle = 0.0f; // Ângulo inicial

     glEnable(GL_DEPTH_TEST);

     // Loop da aplicação - "game loop"
     while (!glfwWindowShouldClose(window))
     {
         // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
         glfwPollEvents();
 
         // Limpa o buffer de cor
         glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         if (incrementScale) {
            incrementScale = false;
            model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
         }

         if (decrementScale) {
            decrementScale = false;
            model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
         }

         // Atualiza rotação acumulativa
         if (rotateW) // Rotação para cima (em torno de X)
         {
             model = rotate(model, radians(1.0f), vec3(1.0f, 0.0f, 0.0f));
         }
         else if (rotateS) // Rotação para baixo (em torno de X)
         {
             model = rotate(model, radians(-1.0f), vec3(1.0f, 0.0f, 0.0f));
         }
         else if (rotateA) // Rotação para a esquerda (em torno de Y)
         {
             model = rotate(model, radians(1.0f), vec3(0.0f, 1.0f, 0.0f));
         }
         else if (rotateD) // Rotação para a direita (em torno de Y)
         {
             model = rotate(model, radians(-1.0f), vec3(0.0f, 1.0f, 0.0f));
         }
         else if (rotateJ) // Rotação em torno de Z (sentido horário)
         {
             model = rotate(model, radians(1.0f), vec3(0.0f, 0.0f, 1.0f));
         }
         else if (rotateI) // Rotação em torno de Z (sentido anti-horário)
         {
             model = rotate(model, radians(-1.0f), vec3(0.0f, 0.0f, 1.0f));
         }
 
         glBindVertexArray(VAO); // Conectando ao buffer de geometria
         glBindTexture(GL_TEXTURE_2D, texID); //conectando com o buffer de textura que será usado no draw
 
         // Translação
         model = translate(model, pos);
         // Rotação
        //  model = rotate(model, radians(angle), axis);
         // Escala
         model = scale(model, dimensions);
         glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, value_ptr(model));
     
         //glUniform4f(glGetUniformLocation(shaderID, "inputColor"), color.r, color.g, color.b, 1.0f); // enviando cor para variável uniform inputColor
                                                                                                     //  Chamada de desenho - drawcall
                                                                                                     //  Poligono Preenchido - GL_TRIANGLES
         glDrawArrays(GL_TRIANGLES, 0, (positions.size() / 3));
     
         glBindVertexArray(0); // Desconectando o buffer de geometria
 
         // Troca os buffers da tela
         glfwSwapBuffers(window);
     }
     // Pede pra OpenGL desalocar os buffers
     glDeleteVertexArrays(1, &VAO);
     // Finaliza a execução da GLFW, limpando os recursos alocados por ela
     glfwTerminate();
     return 0;
 }
 
 // Função de callback de teclado - só pode ter uma instância (deve ser estática se
 // estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
 // ou solta via GLFW
 void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
 {
     if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
         glfwSetWindowShouldClose(window, GL_TRUE);
 
     if (key == GLFW_KEY_W) {
         if (action == GLFW_PRESS) {
             rotateW = true;
         }
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
         decrementScale = true;
     }
 
     if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
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
    glGenBuffers(2, VBO);

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
 
     //glUniform4f(glGetUniformLocation(shaderID, "inputColor"), color.r, color.g, color.b, 1.0f); // enviando cor para variável uniform inputColor
                                                                                                 //  Chamada de desenho - drawcall
                                                                                                 //  Poligono Preenchido - GL_TRIANGLES
     glDrawArrays(GL_TRIANGLES, 0, (positions.size() / 3));
 }
 
 GLuint generateSphere(float radius, int latSegments, int lonSegments, int &nVertices) {
     vector<GLfloat> vBuffer; // Posição + Cor + Normal + UV
 
     vec3 color = vec3(1.0f, 0.0f, 0.0f); // Laranja
 
     auto calcPosUVNormal = [&](int lat, int lon, vec3& pos, vec2& uv, vec3& normal) {
         float theta = lat * pi<float>() / latSegments;
         float phi = lon * 2.0f * pi<float>() / lonSegments;
 
         pos = vec3(
             radius * cos(phi) * sin(theta),
             radius * cos(theta),
             radius * sin(phi) * sin(theta)
         );
 
         uv = vec2(
             phi / (2.0f * pi<float>()),  // u
             theta / pi<float>()          // v
         );
 
         // Normal é a posição normalizada (posição/radius)
         normal = normalize(pos);
     };
 
     for (int i = 0; i < latSegments; ++i) {
         for (int j = 0; j < lonSegments; ++j) {
             vec3 v0, v1, v2, v3;
             vec2 uv0, uv1, uv2, uv3;
             vec3 n0, n1, n2, n3;
 
             calcPosUVNormal(i, j, v0, uv0, n0);
             calcPosUVNormal(i + 1, j, v1, uv1, n1);
             calcPosUVNormal(i, j + 1, v2, uv2, n2);
             calcPosUVNormal(i + 1, j + 1, v3, uv3, n3);
 
             // Primeiro triângulo
             vBuffer.insert(vBuffer.end(), { v0.x, v0.y, v0.z, color.r, color.g, color.b, n0.x, n0.y, n0.z, uv0.x, uv0.y });
             vBuffer.insert(vBuffer.end(), { v1.x, v1.y, v1.z, color.r, color.g, color.b, n1.x, n1.y, n1.z, uv1.x, uv1.y });
             vBuffer.insert(vBuffer.end(), { v2.x, v2.y, v2.z, color.r, color.g, color.b, n2.x, n2.y, n2.z, uv2.x, uv2.y });
 
             // Segundo triângulo
             vBuffer.insert(vBuffer.end(), { v1.x, v1.y, v1.z, color.r, color.g, color.b, n1.x, n1.y, n1.z, uv1.x, uv1.y });
             vBuffer.insert(vBuffer.end(), { v3.x, v3.y, v3.z, color.r, color.g, color.b, n3.x, n3.y, n3.z, uv3.x, uv3.y });
             vBuffer.insert(vBuffer.end(), { v2.x, v2.y, v2.z, color.r, color.g, color.b, n2.x, n2.y, n2.z, uv2.x, uv2.y });
         }
     }
 
     // Criar VAO e VBO
     GLuint VAO, VBO;
     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
 
     glBindVertexArray(VAO);
 
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);
 
     cout << "positions: " << positions.size() * sizeof(GLfloat) << endl;

     // Layout da posição (location 0)
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(0));
     glEnableVertexAttribArray(0);
 
     cout << "textureCoords: " << textureCoords.size() * sizeof(GLfloat) << endl;

     // Layout da cor (location 1)
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
     glEnableVertexAttribArray(1);
 
     // Layout da normal (location 2)
     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
     glEnableVertexAttribArray(2);

     cout << "normals: " << normals.size() * sizeof(GLfloat) << endl;

     // Layout da UV (location 3)
     glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(9 * sizeof(GLfloat)));
     glEnableVertexAttribArray(3);
 
     glBindVertexArray(0);
 
     nVertices = vBuffer.size() / 11; // Cada vértice agora tem 11 floats!
 
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
