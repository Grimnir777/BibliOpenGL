#pragma once
//Basic stuff
#include <iostream>
#include <vector>
#include <math.h>
#include <map>

//OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Custom
#include "Data.h"
#include "Shader.h"
#include "Camera.h"

//Others
#include "sqlite3.h"

using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
// Settings
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 900;

// Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Camera
Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

class App
{
public:
    App() {
        NB_segments = 120;
    }
    void run()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Data project", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        glEnable(GL_DEPTH_TEST);



        double time = 0, dt;// Current time and enlapsed time
        double turn = 0;    // Model turn counter
        double speed = 0.3; // Model rotation speed
        int wire = 0;       // Draw model in wireframe?
        float bgColor[] = { 0.1f, 0.2f, 0.4f };         // Background color 
        unsigned char cubeColor[] = { 255, 0, 0, 128 }; // Model color (32bits RGBA)




        Shader objectShader("basic_shader.vs", "basic_shader.fs");

        Shader lampShader("lighting_shader.vs", "lighting_shader.fs");




        // set up vertex data (and buffer(s)) and configure vertex attributes
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
        };
        // first, configure the cube's VAO (and VBO)
        unsigned int VBO, cubeVAO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
        unsigned int lightVAO;
        glGenVertexArrays(1, &lightVAO);
        glBindVertexArray(lightVAO);

        // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);


        addArc(0, 45, 0.5, 0.2, 0.2);
        //addArc(50, 45, 0.5, 0.2, 0.2);
        //addArc(100, 45, 0.5, 0.2, 0.2);
        //addArc(150, 45, 0.5, 0.2, 0.2);

        objectShader.use();

        //Render loop
        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

            // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);

            // also draw the lamp object
            lampShader.use();
            lampShader.setMat4("projection", projection);
            lampShader.setMat4("view", view);
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            lampShader.setMat4("model", model);

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            renderAll(objectShader);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &lightVAO);
        glDeleteBuffers(1, &VBO);

        glfwTerminate();
    }

private:
    // VARIABLES

    // Data data = Data("articles_data.db");
    int NB_segments;

    vector<unsigned int> VBOsArc;
    vector<unsigned int> VAOsArc;
    vector<unsigned int> EBOsArc;



    vector<glm::vec3> fillVerticesArc(float offsetAngle, float angle, float radius, float prof, float high) {
        float radiusWithProf = radius + prof;
        float realOffset = 3.1415926f * offsetAngle / 180;
        float realAngle = 2.0f * 3.1415926f * angle / 180;
        vector<glm::vec3> points;
        vector<glm::vec3> finalVertices;
        for (int i = 0; i < NB_segments; i++)
        {
            points.push_back(glm::vec3((radius * cos((i * realAngle / NB_segments) + realOffset)), (radius * sin((i * realAngle / NB_segments) + realOffset)), 0.0f));
            points.push_back(glm::vec3((radiusWithProf * cos((i * realAngle / NB_segments) + realOffset)), (radiusWithProf * sin((i * realAngle / NB_segments) + realOffset)), 0.0f));
            points.push_back(glm::vec3((radius * cos((i * realAngle / NB_segments) + realOffset)), (radius * sin((i * realAngle / NB_segments) + realOffset)), high));
            points.push_back(glm::vec3((radiusWithProf * cos((i * realAngle / NB_segments) + realOffset)), (radiusWithProf * sin((i * realAngle / NB_segments) + realOffset)), high));
        }

        //Side of arc #1
        glm::vec3 n = this->normal(points.at(2) - points.at(0), points.at(1) - points.at(0));
        // Fill
        finalVertices.push_back(points.at(0));
        finalVertices.push_back(n);

        finalVertices.push_back(points.at(1));
        finalVertices.push_back(n);

        finalVertices.push_back(points.at(2));
        finalVertices.push_back(n);

        n = this->normal(points.at(1) - points.at(3), points.at(2) - points.at(3));
        // Fill
        finalVertices.push_back(points.at(1));
        finalVertices.push_back(n);

        finalVertices.push_back(points.at(2));
        finalVertices.push_back(n);

        finalVertices.push_back(points.at(3));
        finalVertices.push_back(n);



        for (int i = 0; i < NB_segments - 1 ; i++)
        {
            cout << "itération : " << i << endl;
            // Pour 1 Segment : 8 triangles
                // Pour 1 triangle : 3 vertices 
                    // Pour 1 vertices 3 composants x,y,z et 3 comp de la normale x,y,z
            //1st triangle 0 4 1
            // calcul normal
            glm::vec3 n = this->normal(points.at((i * 4) + 1) - points.at((i * 4) + 0), points.at((i * 4) + 4) - points.at((i * 4) + 0));
            // Fill
            finalVertices.push_back(points.at((i * 4) + 0));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 4));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 1));
            finalVertices.push_back(n);

            // 2nd triangle 1 4 5
            n = this->normal(points.at((i * 4) + 4) - points.at((i * 4) + 5), points.at((i * 4) + 1) - points.at((i * 4) + 5));
            // Fill
            finalVertices.push_back(points.at((i * 4) + 1));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 4));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 5));
            finalVertices.push_back(n);

            // 3rd triangle 5 1 3
            n = this->normal(points.at((i * 4) + 5) - points.at((i * 4) + 1), points.at((i * 4) + 3) - points.at((i * 4) + 1));
            // Fill
            finalVertices.push_back(points.at((i * 4) + 5));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 1));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 3));
            finalVertices.push_back(n);

            // 4th triangle 3 5 7
            n = this->normal(points.at((i * 4) + 3) - points.at((i * 4) + 7), points.at((i * 4) + 5) - points.at((i * 4) + 7));
            // Fill
            finalVertices.push_back(points.at((i * 4) + 3));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 5));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 7));
            finalVertices.push_back(n);


            // 5th triangle 7 6 3
            n = this->normal(points.at((i * 4) + 6) - points.at((i * 4) + 7), points.at((i * 4) + 3) - points.at((i * 4) + 7));
            // Fill
            finalVertices.push_back(points.at((i * 4) + 7));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 6));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 3));
            finalVertices.push_back(n);


            // 6th triangle 3 6 2
            n = this->normal(points.at((i * 4) + 3) - points.at((i * 4) + 2), points.at((i * 4) + 6) - points.at((i * 4) + 2));
            // Fill
            finalVertices.push_back(points.at((i * 4) + 3));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 6));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 2));
            finalVertices.push_back(n);

            // 7th triangle 2 6 4
            n = this->normal(points.at((i * 4) + 2) - points.at((i * 4) + 6), points.at((i * 4) + 4) - points.at((i * 4) + 6));
            // Fill
            finalVertices.push_back(points.at((i * 4) + 2));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 6));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 4));
            finalVertices.push_back(n);

            // 8th triangle 4 2 0
            n = this->normal(points.at((i * 4) + 4) - points.at((i * 4) + 0), points.at((i * 4) + 2) - points.at((i * 4) + 0));
            // Fill
            finalVertices.push_back(points.at((i * 4) + 4));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 2));
            finalVertices.push_back(n);

            finalVertices.push_back(points.at((i * 4) + 0));
            finalVertices.push_back(n);

        }

        //Side of arc #1
        n = this->normal(points.at(((NB_segments-1) * 4) + 1) - points.at(((NB_segments - 1) * 4) + 0), points.at(((NB_segments - 1) * 4) + 2) - points.at(((NB_segments - 1) * 4) + 0));
        // Fill
        finalVertices.push_back(points.at(((NB_segments - 1) * 4) + 0));
        finalVertices.push_back(n);

        finalVertices.push_back(points.at(((NB_segments - 1) * 4) + 1));
        finalVertices.push_back(n);

        finalVertices.push_back(points.at(((NB_segments - 1) * 4) + 2));
        finalVertices.push_back(n);

        n = this->normal(points.at(((NB_segments - 1) * 4) + 2) - points.at(((NB_segments - 1) * 4) + 3), points.at(((NB_segments - 1) * 4) + 1) - points.at(((NB_segments - 1) * 4) + 3));
        // Fill
        finalVertices.push_back(points.at(((NB_segments - 1) * 4) + 1));
        finalVertices.push_back(n);

        finalVertices.push_back(points.at(((NB_segments - 1) * 4) + 2));
        finalVertices.push_back(n);

        finalVertices.push_back(points.at(((NB_segments - 1) * 4) + 3));
        finalVertices.push_back(n);

        return finalVertices;

    }

    glm::vec3 normal(glm::vec3 v1, glm::vec3 v2) {
        return glm::normalize(glm::cross(v1, v2));
    }

    void mapVerticesToBuffers(float* vertices, int size_of_vertices, int* indexes, int size_of_indexes) {
        glGenBuffers(1, &this->VBOsArc.back());
        glGenBuffers(1, &this->EBOsArc.back());
        glGenVertexArrays(1, &this->VAOsArc.back());

        glBindBuffer(GL_ARRAY_BUFFER, this->VBOsArc.back());
        glBufferData(GL_ARRAY_BUFFER, size_of_vertices, vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBOsArc.back());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_of_indexes, indexes, GL_STATIC_DRAW);

        glBindVertexArray(this->VAOsArc.back());
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        //unbind 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    void addArc(
        float offset_angle,
        float angle,
        float radius,
        float prof,
        float high
    ) {

        this->VBOsArc.push_back(0);
        this->VAOsArc.push_back(0);
       // this->EBOsArc.push_back(0);

        //int size_vertices = this->NB_segments * 3 * 4; // 3D and 4 points per iteration
        //int size_indexes = this->NB_segments * 8 * 3;  // 8 triangles composed by 3 point indexes

       // cout << "Arc params ; size_vertices :  " << size_vertices << " ; size_indexes :  " << size_indexes;
        //float* verticesArc = new float[size_vertices];
        //int* indexesArc = new int[size_indexes];

        vector<glm::vec3> vertices = fillVerticesArc(offset_angle, angle, radius, prof, high);
        glGenBuffers(1, &this->VBOsArc.back());
        glGenVertexArrays(1, &this->VAOsArc.back());

        glBindBuffer(GL_ARRAY_BUFFER, this->VBOsArc.back());
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);


        glBindVertexArray(this->VAOsArc.back());

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //unbind 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        /*
        mapVerticesToBuffers(
            verticesArc,
            (size_vertices * sizeof(float)),
            indexesArc,
            (size_indexes * sizeof(int)));
        */
    }

    void renderAll(Shader ourShader) {
        ourShader.use();
        ourShader.setVec3("objectColor", glm::vec3(0.34f, 0.27f, 0.48f));
        ourShader.setVec3("lightColor", glm::vec3(1.0f));
        ourShader.setVec3("lightPos", lightPos);
        ourShader.setVec3("viewPos", camera.Position);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        for (int i = 0; i < VBOsArc.size(); i++)
        {
            glBindVertexArray(VAOsArc.at(i));
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOsArc.at(i));

            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            float angle = 0;
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 3*8 * 120);
            //glDrawElements(GL_TRIANGLES, (NB_segments * 24), GL_UNSIGNED_INT, 0);
        }
    }

};



/*
    -- External --
    Window and camera callbacks
*/
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
