#pragma once
#include "Data.h"

class App
{
public:
	App();
	~App();
	void run();
private:
	// OpenGL functions
	void compileShaders(
		int* vertexShader,
		int* fragmentShader,
		unsigned int* shaderProgram);

	// Window
	//void processInput(GLFWwindow* window);
	//void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	void mapVerticesToBuffers(
		float* vertices, 
		int size_of_vertices, 
		int* indexes, 
		int size_of_indexes, 
		unsigned int* VBO, 
		unsigned int* VAO, 
		unsigned int* EBO);
	void renderAll();
	// Vertices fill
	void addCircle(
		int nb_segments,
		float radius);
	void addArc(
		int nb_segments,
		float radius,
		float prof,
		float offset_angle,
		float angle);


	// Variables
	unsigned int shaderProgram;
	Data data;
	const unsigned int SCR_WIDTH = 900;
	const unsigned int SCR_HEIGHT = 900;
	const int NB_segments = 120;
};