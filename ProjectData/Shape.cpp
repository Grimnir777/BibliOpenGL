#include "Shape.h"
#include <iostream>
#include <glad/glad.h>

Shape::Shape(unsigned int shader_program)
{
	std::cout << "Shape constructor called";
	this->VBO = 0;
	this->VAO = 0;
	this->EBO = 0;
	//glGenBuffers(1, &this->VBO);
	//glGenVertexArrays(1, &this->VAO);
	//glGenBuffers(1, &this->EBO);
	this->shader_program = shader_program;
}