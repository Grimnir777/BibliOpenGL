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

#include <ft2build.h>
#include FT_FREETYPE_H

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


// Settings
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 900;

// Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Camera
Camera camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(0.0f, 0.0f, 5.0f);


class App
{
public:
    App() {
        NB_segments = 10;
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


        // Set OpenGL options
        //glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        this->actualNode = this->data.rootNode;
        fillScene(this->actualNode, 0.5, 0.5, 0.5);

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

            renderAll(objectShader);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
    }

private:
    // Variables
    Data data = Data("articles_data.db");
    int NB_segments;
    float angleToReach = 0.0f;
    float actualAngle = 0.0f;

    vector<unsigned int> VBOsArc;
    vector<unsigned int> VAOsArc;
    vector<glm::vec3> colors;
    Node* actualNode;
    int year = 2015;

    vector<glm::vec3> fillVerticesArc(float offsetAngle, float angle, float radius, float prof,float startHigh, float high) {
        float radiusWithProf = radius + prof;
        float realOffset = 3.1415926f * offsetAngle / 180;
        float realAngle = 3.1415926f * angle / 180;
        vector<glm::vec3> points;
        vector<glm::vec3> finalVertices;
        for (int i = 0; i < NB_segments; i++)
        {
            points.push_back(glm::vec3(
                (radius * cos((i * realAngle / NB_segments) + realOffset)),
                (radius * sin((i * realAngle / NB_segments) + realOffset)),
                startHigh));
            points.push_back(glm::vec3(
                (radiusWithProf * cos((i * realAngle / NB_segments) + realOffset)),
                (radiusWithProf * sin((i * realAngle / NB_segments) + realOffset)),
                startHigh));
            points.push_back(glm::vec3(
                (radius * cos((i * realAngle / NB_segments) + realOffset)),
                (radius * sin((i * realAngle / NB_segments) + realOffset)),
                high+ startHigh));
            points.push_back(glm::vec3(
                (radiusWithProf * cos((i * realAngle / NB_segments) + realOffset)),
                (radiusWithProf * sin((i * realAngle / NB_segments) + realOffset)),
                high + startHigh));
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

        //Side of arc #last One
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

    void addArc(
        float offset_angle,
        float angle,
        float radius,
        float prof,
        float startHigh,
        float high
    ) {

        this->VBOsArc.push_back(0);
        this->VAOsArc.push_back(0);

        vector<glm::vec3> vertices = fillVerticesArc(offset_angle, angle, radius, prof, startHigh, high);
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
    }

    float makeArcs(
        float startAngle,
        float endAngle,
        float radius,
        float prof,
        float startHigh,
        float high) {
        int nbArc = this->actualNode->children.size();
        float arcAngle = (float)(endAngle - startAngle) / nbArc;
        for (int i = 0; i < nbArc; i++)
        {
            addArc((i * arcAngle) + startAngle, (arcAngle)-(arcAngle / 5), radius, prof, startHigh, high);
            this->colors.push_back(this->actualNode->children[i]->color);
        }
        return arcAngle;
    }

    void makeArcsLeaf(
        float startAngle,
        float endAngle,
        float radius,
        float startHigh,
        float high) {
        int nbArc = this->actualNode->children.size();
        float arcAngle = (float)(endAngle - startAngle) / nbArc;

        int min = 10000;
        int max = -10000;
        for (int i = 0; i < nbArc; i++)
        {
            int val = this->actualNode->children[i]->count;
            if (val > max) max = val;
            if (val < min) min = val;
        }
        cout << max << endl;
        cout << min << endl;
        int divide = (max - min);
        for (int i = 0; i < nbArc; i++)
        {
            float countNormalized = (float) 0.5 * (this->actualNode->children[i]->count - min) / divide;
            cout << countNormalized << endl;
            addArc((i * arcAngle) + startAngle, (arcAngle)-(arcAngle / 5), radius, countNormalized, startHigh, high);
            this->colors.push_back(this->actualNode->children[i]->color);
        }

    }

    void renderAll(Shader ourShader) {
        ourShader.use();
        
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
            ourShader.setVec3("objectColor", this->colors.at(i));
            glBindVertexArray(VAOsArc.at(i));

            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            if (this->angleToReach != this->actualAngle) {
                cout << "To reach" << this->angleToReach << endl;
                cout << "actual" << this->actualAngle << endl;
                if (this->actualAngle < this->angleToReach) {
                    this->actualAngle += 1;
                }
                else {
                    this->actualAngle -= 1;
                }
            }
            model = glm::rotate(model, glm::radians(this->actualAngle), glm::vec3(0.0f, 0.0f, 1.0f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 3*8 * 120);
        }
    }
    void fillScene(
        Node* node,
        float radius,
        float prof,
        float high)
    {

        int levSize = node->children.size();
        if (levSize > 0) {
            if (node->children[0]->IsLeaf()) {
                makeArcsLeaf(0, 360, radius, 0.0f, high);
            }
            else {
                makeArcs(0, 360, radius, prof, 0.0f, high);
            }
        }
    }
    void clearBuffers() {
        for (size_t i = 0; i < this->VAOsArc.size(); i++)
        {
            glDeleteVertexArrays(1, &this->VAOsArc[i]);
        }
        for (size_t i = 0; i < this->VBOsArc.size(); i++)
        {
            glDeleteBuffers(1, &this->VBOsArc[i]);
        }
    }
    
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
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        {
            if (this->actualNode->IsLeaf())
            {
                cout << "You are at lead level you cannot go forward" << endl;
            }
            else {
                cout << "Please choose a node to visit by its index: " << endl;
                for (int i = 0; i < this->actualNode->children.size(); i++)
                {
                    cout <<  "[" << i << "] : " << this->actualNode->children[i]->value << endl;
                }
                int choice;
                cout << "Please enter your choice : ";
                cin >> choice;
                this->actualNode = this->actualNode->children[choice];
                this->clearBuffers();
                cout << "Refilling the vertices ..." << endl;
                this->fillScene(this->actualNode, 0.5, 0.5, 0.5);
                cout << "Scene ready !" << endl;
            }
            
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            if (this->actualNode->IsRoot()) 
            {
                cout << "You are at root level you cannot go back" << endl;
            }
            else {
                this->actualNode = this->actualNode->parent;
                cout << "Back to the previous level : " << this->actualNode->value << endl;
            }
            cout << "Refilling the vertices ..." << endl;
            this->clearBuffers();
            this->fillScene(this->actualNode, 0.5, 0.5, 0.5);
            cout << "Scene ready !" << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        {
            cout << "--- Help ---" << endl;
            cout << "Type Z/Q/S/D to move yourself into the scene" << endl;
            cout << "You can also zoom with the mouse wheel and look around with the mouse\n" << endl;
            cout << "Type G to choose a new node to visit" << endl;
            cout << "Type B to go to previous node visited" << endl;
            cout << "Type I to print infos about current visualization" << endl;
            cout << "--- Help ---" << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        {
            cout << "Infos about current state : " << endl;
            for (int i = 0; i < this->actualNode->children.size(); i++)
            {
                if (this->actualNode->children[i]->count != -1) {
                    cout << "[" << i << "] ; Name : " << this->actualNode->children[i]->value << " ; Value : " << this->actualNode->children[i]->count << endl;
                }
                else {
                    cout << "[" << i << "] ; Name : " << this->actualNode->children[i]->value << endl;
                }
            }
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            this->year++;
            cout << "Next year : " << this->year << endl;
            this->data.refill_arrays(this->year);
            this->actualNode = this->data.rootNode;
            cout << "Refilling the vertices ..." << endl;
            this->clearBuffers();
            this->fillScene(this->actualNode, 0.5, 0.5, 0.5);
            cout << "Scene ready !" << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            this->year--;
            cout << "Previous year : " << this->year << endl;
            this->data.refill_arrays(this->year);
            this->actualNode = this->data.rootNode;
            cout << "Refilling the vertices ..." << endl;
            this->clearBuffers();
            this->fillScene(this->actualNode, 0.5, 0.5, 0.5);
            cout << "Scene ready !" << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE) {
            if (this->angleToReach < 360) {
                this->angleToReach += 1;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE) {
            if (this->angleToReach > 0) {
                this->angleToReach -= 1;
            }
        }
            
    }

};

/*
    -- External --
    Window and camera callbacks
*/

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