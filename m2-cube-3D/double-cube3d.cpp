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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int setupShader();
int setupGeometry();

const GLuint WIDTH = 1000, HEIGHT = 1000;

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
"   gl_Position = model * vec4(position, 1.0);\n"
"   finalColor = vec4(color, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"   color = finalColor;\n"
"}\n\0";

float scale = 1.0f;
bool needScale = false;
bool incrementScale = false;
bool decrementScale = false;
bool rotateW=false, rotateS=false, rotateA=false, rotateD=false, rotateI=false, rotateJ=false;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Olá 3D - Matheus Porto!", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint shaderID = setupShader();
    GLuint VAO = setupGeometry();
    glUseProgram(shaderID);

    GLint modelLoc = glGetUniformLocation(shaderID, "model");
    glEnable(GL_DEPTH_TEST);

    glm::mat4 models[2];
    for (int i = 0; i < 2; i++) {
        models[i] = glm::mat4(1.0f);
        models[i] = glm::scale(models[i], glm::vec3(0.5f, 0.5f, 0.5f));
        models[i] = glm::translate(models[i], glm::vec3(i * 1.2f - 0.5f, 0.0f, 0.0f));
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        for (int i = 0; i < 2; i++) {
            if (incrementScale) {
                models[i] = glm::scale(models[i], glm::vec3(1.1f, 1.1f, 1.1f));
                incrementScale = false;
            }

            if (decrementScale) {
                models[i] = glm::scale(models[i], glm::vec3(0.9f, 0.9f, 0.9f));
                decrementScale = false;
            }

            float angleX=0.1f, angleY=0.1f, angleZ=0.1f;
            if (rotateW)
            {
                models[i] = glm::rotate(models[i], angleX, glm::vec3(1.0f, 0.0f, 0.0f));
            }
            else if (rotateS) 
            {
                models[i] = glm::rotate(models[i], angleX, glm::vec3(-1.0f, 0.0f, 0.0f));
            }
            else if (rotateI)
            {
                models[i] = glm::rotate(models[i], angleY, glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (rotateJ)
            {
                models[i] = glm::rotate(models[i], angleY, glm::vec3(0.0f, -1.0f, 0.0f));
            }
            else if (rotateA)
            {
                models[i] = glm::rotate(models[i], angleZ, glm::vec3(0.0f, 0.0f, 1.0f));
            }
            else if (rotateD)
            {
                models[i] = glm::rotate(models[i], angleZ, glm::vec3(0.0f, 0.0f, -1.0f));
            }

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(models[i]));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

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

int setupShader() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int setupGeometry() {
    GLfloat vertices[] = {
 
        // Quadrado amarelo
        //x     y     z    r    g    b
        -0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
        -0.5, -0.5,  0.5, 1.0, 1.0, 0.0,
         0.5, -0.5, -0.5, 1.0, 1.0, 0.0,

        -0.5, -0.5,  0.5, 1.0, 1.0, 0.0,
         0.5, -0.5,  0.5, 1.0, 1.0, 0.0,
         0.5, -0.5, -0.5, 1.0, 1.0, 0.0,

        // Quadrado azul claro
        //x     y     z    r    g    b
        -0.5, 0.5,  0.5, 0.0, 1.0, 1.0,
        -0.5, 0.5, -0.5, 0.0, 1.0, 1.0,
         0.5, 0.5,  0.5, 0.0, 1.0, 1.0,

        -0.5, 0.5, -0.5, 0.0, 1.0, 1.0,
         0.5, 0.5, -0.5, 0.0, 1.0, 1.0,
         0.5, 0.5,  0.5, 0.0, 1.0, 1.0,

        // Quadrado vermelho
        //x     y      z    r    g    b
        -0.5, -0.5,  -0.5, 1.0, 0.0, 0.0,
        -0.5,  0.5,  -0.5, 1.0, 0.0, 0.0,
         0.5, -0.5,  -0.5, 1.0, 0.0, 0.0,

        -0.5,  0.5, -0.5, 1.0, 0.0, 0.0,
         0.5,  0.5, -0.5, 1.0, 0.0, 0.0,
         0.5, -0.5, -0.5, 1.0, 0.0, 0.0,
 
        // Quadrado rosa
        //x     y     z    r    g    b
        -0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
        -0.5,  0.5, -0.5, 1.0, 0.0, 1.0,
        -0.5, -0.5,  0.5, 1.0, 0.0, 1.0,

        -0.5,  0.5, 0.5, 1.0, 0.0, 1.0,
        -0.5, -0.5, 0.5, 1.0, 0.0, 1.0,
        -0.5,  0.5,  -0.5, 1.0, 0.0, 1.0,

        // Quadrado verde
        //x    y     z    r    g    b
        0.5,  0.5, -0.5, 0.0, 1.0, 0.0,
        0.5, -0.5, -0.5, 0.0, 1.0, 0.0,
        0.5,  0.5,  0.5, 0.0, 1.0, 0.0,

        0.5, -0.5, 0.5, 0.0, 1.0, 0.0,
        0.5, 0.5, 0.5, 0.0, 1.0, 0.0,
        0.5, -0.5, -0.5, 0.0, 1.0, 0.0,

        // Quadrado azul
        //x     y     z    r    g    b
        -0.5, -0.5,  0.5, 0.0, 0.0, 1.0,
        -0.5,  0.5,  0.5, 0.0, 0.0, 1.0,
         0.5, -0.5,  0.5, 0.0, 0.0, 1.0,

        -0.5,  0.5, 0.5, 0.0, 0.0, 1.0,
         0.5,  0.5, 0.5, 0.0, 0.0, 1.0,
         0.5, -0.5, 0.5, 0.0, 0.0, 1.0,
     };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}