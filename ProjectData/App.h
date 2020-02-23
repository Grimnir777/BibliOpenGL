#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Data.h"
#include "Shader.h"
#include "Camera.h"

#include <iostream>
#include <vector>
#include <math.h>
#include "Arc.h"
#include "Circle.h"
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

        Shader ourShader("basic_shader.vs", "basic_shader.fs");



        addArc(90.0, 180.0, 0.5, 1, 1);

      

        // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
        // -------------------------------------------------------------------------------------------
        ourShader.use();
        // render loop
        // -----------
        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderAll(ourShader);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);

        glfwTerminate();
    }

private:
    // VARIABLES

    // Data data = Data("articles_data.db");
    int NB_segments;
    unsigned int VBO, VAO;

    vector<unsigned int> VBOsArc;
    vector<unsigned int> VAOsArc;
    vector<unsigned int> EBOsArc;



    void fillVerticesArc(float* vertices, int* indexes, float offsetAngle, float angle, float radius, float prof, float high) {
        float radiusWithProf = radius + prof;
        float realOffset = 3.1415926f * offsetAngle / 180;
        float realAngle = 2.0f * 3.1415926f * angle / 180;

        for (int i = 0; i < NB_segments; i++)
        {
            // First point low and near
            vertices[i * 12] = (radius * cos((i * realAngle / NB_segments) + realOffset));
            vertices[(i * 12) + 1] = (radius * sin((i * realAngle / NB_segments) + realOffset));
            vertices[(i * 12) + 2] = 0.0f;

            // Second point low and far
            vertices[(i * 12) + 3] = (radiusWithProf * cos((i * realAngle / NB_segments) + realOffset));
            vertices[(i * 12) + 4] = (radiusWithProf * sin((i * realAngle / NB_segments) + realOffset));
            vertices[(i * 12) + 5] = 0.0f;

            // Third point high and near
            vertices[(i * 12) + 6] = (radius * cos((i * realAngle / NB_segments) + realOffset));
            vertices[(i * 12) + 7] = (radius * sin((i * realAngle / NB_segments) + realOffset));
            vertices[(i * 12) + 8] = high;

            // Second point low and far
            vertices[(i * 12) + 9] = (radiusWithProf * cos((i * realAngle / NB_segments) + realOffset));
            vertices[(i * 12) + 10] = (radiusWithProf * sin((i * realAngle / NB_segments) + realOffset));
            vertices[(i * 12) + 11] = high;


            // Indexes 8*3 = 24 triangles per iteration 
            // 1st triangle
            indexes[(i * 24)] = i;
            indexes[(i * 24) + 1] = i + 1;
            indexes[(i * 24) + 2] = i + 4;

            // 2nd triangle
            indexes[(i * 24) + 3] = i + 1;
            indexes[(i * 24) + 4] = i + 4;
            indexes[(i * 24) + 5] = i + 5;

            // 3rd triangle
            indexes[(i * 24) + 6] = i + 2;
            indexes[(i * 24) + 7] = i + 3;
            indexes[(i * 24) + 8] = i + 6;

            // 4th triangle
            indexes[(i * 24) + 9] = i + 3;
            indexes[(i * 24) + 10] = i + 6;
            indexes[(i * 24) + 11] = i + 7;

            // 5th triangle
            indexes[(i * 24) + 12] = i + 0;
            indexes[(i * 24) + 13] = i + 2;
            indexes[(i * 24) + 14] = i + 4;

            // 6th triangle
            indexes[(i * 24) + 15] = i + 2;
            indexes[(i * 24) + 16] = i + 4;
            indexes[(i * 24) + 17] = i + 6;

            // 7th triangle
            indexes[(i * 24) + 18] = i + 1;
            indexes[(i * 24) + 19] = i + 3;
            indexes[(i * 24) + 20] = i + 5;

            // 8th triangle
            indexes[(i * 24) + 21] = i + 3;
            indexes[(i * 24) + 22] = i + 5;
            indexes[(i * 24) + 23] = i + 7;

        }
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
        this->EBOsArc.push_back(0);

        int size_vertices = this->NB_segments * 3 * 4; // 3D and 4 points per iteration
        int size_indexes = this->NB_segments * 8 * 3;  // 8 triangles composed by 3 point indexes

        cout << "Arc params ; size_vertices :  " << size_vertices << " ; size_indexes :  " << size_indexes;
        float* verticesArc = new float[size_vertices];
        int* indexesArc = new int[size_indexes];

        fillVerticesArc(verticesArc, indexesArc, offset_angle, angle, radius, prof, high);

        mapVerticesToBuffers(
            verticesArc,
            (size_vertices * sizeof(float)),
            indexesArc,
            (size_indexes * sizeof(int)));
    }
    /*
	// OpenGL functions

	void renderAll();
*/
    void renderAll(Shader ourShader) {
        ourShader.use();
        ourShader.setVec4("colorInput", glm::vec4(0.34f, 0.27f, 0.48f, 1.0f));
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        for (int i = 0; i < VBOsArc.size(); i++)
        {
            glBindVertexArray(VAOsArc.at(i));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOsArc.at(i));

            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            float angle = 0;
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setMat4("model", model);

            //glDrawArrays(GL_TRIANGLES, 0, 3*8);
            glDrawElements(GL_TRIANGLES, (NB_segments * 24), GL_UNSIGNED_INT, 0);
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
