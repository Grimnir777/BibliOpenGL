#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>
#include "Arc.h"
#include "Circle.h"
#include "Data.h"
#include "sqlite3.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 900;

//Const for circles
const int NB_segments = 120;

const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"}\0";

const char* fShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 colorInput;\n"
"void main()\n"
"{\n"
"   FragColor = colorInput;\n"
"}\n\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.33f,0.36f,0.43f, 1.0f);\n"
"}\n\0";


void fillVerticesCircle(float *vertices, int* indexes, float radius) {
    float x = 0.0f, y = 0.0f;
    float twicePi = 2.0f * 3.1415926f;

    for (int i = 0; i < NB_segments; i++)
    {
        vertices[i * 2] = x + (radius * cos(i * twicePi / NB_segments));
        vertices[(i * 2) + 1] = y + (radius * sin(i * twicePi / NB_segments));

        indexes[(i * 3)] = i;
        indexes[(i * 3) + 1] = 0;
        indexes[(i * 3) + 2] = i + 1;
        //cout << vertices[i * 2] << "," << vertices[(i * 2) + 1] << "\n";
    }
}

void fillVerticesArc(float *vertices, int *indexes, float offsetAngle, float angle, float radius, float prof, float cx, float cy) {
    float radiusWithProf = radius + prof;
    float realOffset =  3.1415926f * offsetAngle / 180;
    float realAngle = 2.0f * 3.1415926f * angle /180;

    for (int i = 0; i < NB_segments; i++)
    {
        vertices[i * 4]         = cx + (radius * cos( (i * realAngle / NB_segments) + realOffset));
        vertices[(i * 4) + 1]   = cy + (radius * sin( (i * realAngle / NB_segments) + realOffset));
        
        vertices[(i * 4) + 2]   = cx + (radiusWithProf * cos((i * realAngle / NB_segments) + realOffset));
        vertices[(i * 4) + 3]   = cy + (radiusWithProf * sin((i * realAngle / NB_segments) + realOffset));

        indexes[(i * 3)] = i;
        indexes[(i * 3) + 1] = i + 1;
        indexes[(i * 3) + 2] = i + 2;
    }
}


void compileShaders(int *vertexShader, int *fragmentShader, unsigned int *shaderProgram) {
    // Create the vertex shader and compile it
    *vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(*vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(*vertexShader);

    // Create the fragment shader and compile it
    *fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(*fragmentShader, 1, &fShaderSource, NULL);
    glCompileShader(*fragmentShader);

    // Create shader program and link
    *shaderProgram = glCreateProgram();
    glAttachShader(*shaderProgram, *vertexShader);
    glAttachShader(*shaderProgram, *fragmentShader);
    glLinkProgram(*shaderProgram);

    glDeleteShader(*vertexShader);
    glDeleteShader(*fragmentShader);
}

void mapVerticesToBuffers(float* vertices,int size_of_vertices, int* indexes, int size_of_indexes, unsigned int *VBO, unsigned int *VAO, unsigned int *EBO) {
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);
    glGenVertexArrays(1, VAO);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, size_of_vertices, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_of_indexes, indexes, GL_STATIC_DRAW);

    glBindVertexArray(*VAO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //unbind 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderAll(
    vector<unsigned int> VBOsArc,
    vector<unsigned int> VAOsArc,
    vector<unsigned int> EBOsArc,
    vector<unsigned int> VBOsCircle,
    vector<unsigned int> VAOsCircle,
    vector<unsigned int> EBOsCircle,
    unsigned int shaderProgram) {
    glUseProgram(shaderProgram);
    for (int i = 0; i < VBOsArc.size(); i++)
    {
        glUniform4f(glGetUniformLocation(shaderProgram, "colorInput"), 0.34f, 0.27f, 0.48f, 1.0f);
        glBindVertexArray(VAOsArc.at(i));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOsArc.at(i));
        glDrawElements(GL_TRIANGLES, (NB_segments * 5), GL_UNSIGNED_INT, 0);
    }
    for (int i = 0; i < VBOsCircle.size(); i++)
    {
        glUniform4f(glGetUniformLocation(shaderProgram, "colorInput"), 0.56f, 0.27f, 0.41f, 1.0f);
        glBindVertexArray(VAOsCircle.at(i));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOsCircle.at(i));
        glDrawElements(GL_TRIANGLES, (NB_segments * 3), GL_UNSIGNED_INT, 0);
    }
}

void addCircle(
    vector<unsigned int> *VBOs,
    vector<unsigned int> *VAOs,
    vector<unsigned int> *EBOs,
    int nb_segments,
    float radius) {

    VBOs->push_back(0);
    VAOs->push_back(0);
    EBOs->push_back(0);

    int size_vertices = nb_segments * 2;
    int size_indexes = nb_segments * 3;
    // 120 steps and 2 values for each step + center
    float* verticesCircle = new float[size_vertices];
    int* indexesCircle = new int[size_indexes];

    fillVerticesCircle(verticesCircle, indexesCircle, radius);

    mapVerticesToBuffers(
        verticesCircle,
        size_vertices * sizeof(float),
        indexesCircle,
        size_indexes * sizeof(int),
        &VBOs->back(),
        &VAOs->back(),
        &EBOs->back());
}

void addArc(
    vector<unsigned int>* VBOs,
    vector<unsigned int>* VAOs,
    vector<unsigned int>* EBOs,
    int nb_segments,
    float radius,
    float prof,
    float offset_angle,
    float angle) {

    VBOs->push_back(0);
    VAOs->push_back(0);
    EBOs->push_back(0);

    int size_vertices = nb_segments * 4;
    int size_indexes = nb_segments * 3;

    cout << "size of vertices " << size_vertices * sizeof(float)  << endl;
    cout << "size of indexes " << size_indexes * sizeof(int) << endl;

    float* verticesArc = new float[size_vertices];
    int* indexesArc = new int[size_indexes];

    fillVerticesArc(verticesArc, indexesArc, offset_angle, angle, radius, prof, 0, 0);

    mapVerticesToBuffers(
        verticesArc,
        (size_vertices * sizeof(float)),
        indexesArc,
        (size_indexes * sizeof(int)),
        &VBOs->back(),
        &VAOs->back(),
        &EBOs->back());
}

// to make random value between 0 and 1
//static_cast <float> (rand()) / static_cast <float> (RAND_MAX)


void makeArcs(
    vector<unsigned int>* VBOs,
    vector<unsigned int>* VAOs,
    vector<unsigned int>* EBOs,
    int nbArc) {
    float arcAngle = (float) 360 / nbArc;
    cout << arcAngle << endl;
    for (int i = 0; i < nbArc; i++)
    {
        addArc(VBOs, VAOs, EBOs, NB_segments, 0.22, 0.7, (i * arcAngle) + 1, arcAngle - 1);
    }
}

int main()
{
    Data data = Data("articles_data.db");
    /*
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Data project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Initialization shaders
    int vertexShader, fragmentShader;
    unsigned int shaderProgram;
    compileShaders(&vertexShader, &fragmentShader, &shaderProgram);

    vector<unsigned int> VBOsCircle;
    vector<unsigned int> VAOsCircle;
    vector<unsigned int> EBOsCircle;
    
    vector<unsigned int> VBOsArc;
    vector<unsigned int> VAOsArc;
    vector<unsigned int> EBOsArc;


    addCircle(&VBOsCircle, &VAOsCircle, &EBOsCircle, NB_segments, 0.2);
    makeArcs(&VBOsArc, &VAOsArc, &EBOsArc, 111 );

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0,0,0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderAll(VBOsArc,VAOsArc,EBOsArc, VBOsCircle, VAOsCircle, EBOsCircle, shaderProgram);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    */
    do
    {
        cout << '\n' << "Press a key to continue...";
    } while (cin.get() != '\n');

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        cout << "key left pressed" << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        cout << "key right pressed" << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        cout << "key up pressed" << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        cout << "key down pressed" << endl;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}