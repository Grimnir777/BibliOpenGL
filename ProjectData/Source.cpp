#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>
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
        cout << vertices[i * 2] << "," << vertices[(i * 2) + 1] << "\n";
    }
    vertices[360] = 0.5f;
    vertices[361] = 0.0f;
}

void fillVerticesArc(float *vertices, int *indexes, float offsetAngle, float angle, float radius, float prof, float cx, float cy) {
    float radiusWithProf = radius + prof;
    float realOffset = 2.0f * 3.1415926f * offsetAngle / 360;
    float realAngle = 2.0f * 3.1415926f * angle /360;

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
    vertices[720] = 0.5f;
    vertices[720] = 0.0f;
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

void renderArc(float *verticesArc, int *indexesArc) {
    unsigned int VBOArc = 0, VAOArc = 0, EBOArc = 0;
    glGenBuffers(1, &VBOArc);
    glGenBuffers(1, &EBOArc);
    glGenVertexArrays(1, &VAOArc);

    glBindBuffer(GL_ARRAY_BUFFER, VBOArc);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesArc), verticesArc, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOArc);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexesArc), indexesArc, GL_STATIC_DRAW);

    glBindVertexArray(VAOArc);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //unbind 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void updateBuffer(int *VBO) {
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
}



int main()
{
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

    // 120 steps and 2 values for each step + center
    float verticesArc[242];
    int indexesArc[360];
    fillVerticesCircle(verticesArc,indexesArc, 0.45);


    unsigned int VBOArc = 0, VAOArc = 0, EBOArc = 0;
    glGenBuffers(1, &VBOArc);
    glGenBuffers(1, &EBOArc);
    glGenVertexArrays(1, &VAOArc);

    glBindBuffer(GL_ARRAY_BUFFER, VBOArc);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesArc), verticesArc, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOArc);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexesArc), indexesArc, GL_STATIC_DRAW);

    glBindVertexArray(VAOArc);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //unbind 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Arc
    float vertices[482];
    int indexes[720];
    fillVerticesArc(vertices, indexes, 40, 260, 0.5, 0.2, 0, 0);

    unsigned int VBO=0, VAO=0, EBO=0;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //unbind 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
   

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0,0,0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        // Circle Render
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "colorInput");
        glUniform4f(vertexColorLocation, 0.33f, 0.36f, 0.43f, 1.0f);
        glBindVertexArray(VAOArc);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOArc);
        glDrawElements(GL_TRIANGLES, (NB_segments * 3)-1, GL_UNSIGNED_INT, 0);


        //Arc render
        glUseProgram(shaderProgram);
        int vertexColorLocation2 = glGetUniformLocation(shaderProgram, "colorInput");
        glUniform4f(vertexColorLocation2, 0.34f, 0.27f, 0.48f, 1.0f);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, NB_segments * 5, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
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