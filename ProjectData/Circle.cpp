#include "Circle.h"
#include "Shape.h"
#include <math.h>
#include <iostream>
#include <glad/glad.h>
using namespace std;

Circle::Circle(
	unsigned int shader_program,
	int nb_segments,
	float radius,
	float cx,
	float cy) :Shape(shader_program)
{
	this->nb_segments = nb_segments;
	this->radius = radius;
	this->cx = cx;
	this->cy = cy;
	this->size_of_vertices = nb_segments * 2 * sizeof(float);
	this->size_of_indexes = nb_segments * 3 * sizeof(float);
	//this->vertices = new float[this->size_of_vertices];
	//this->indexes = new float[this->size_of_indexes];
}

Circle::~Circle()
{
}

void Circle::updateVertices() {
	float twicePi = 2.0f * 3.1415926f;

	for (int i = 0; i < this->nb_segments; i++)
	{
		this->vertices[i * 2] = this->cx + (this->radius * cos(i * twicePi / this->nb_segments));
		this->vertices[(i * 2) + 1] = this->cy + (this->radius * sin(i * twicePi / this->nb_segments));

		this->indexes[(i * 3)] = i;
		this->indexes[(i * 3) + 1] = 0;
		this->indexes[(i * 3) + 2] = i + 1;
		cout << "[" << i << "]" << vertices[i * 2] << "," << vertices[(i * 2) + 1] << "\n";
	}
	this->vertices[0] = 0.0f;
	this->vertices[1] = 0.0f;
}

void Circle::updateBuffers()
{
	std::cout << "size of vertices" << this->size_of_vertices << "\n";
	std::cout << "size of indexes" << this->size_of_indexes << "\n";
	// Fill VBO : Vertex Buffer objects with vertices
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->size_of_vertices, this->vertices, GL_STATIC_DRAW);

	// Fill VAO : Vertex Array Object
	glBindVertexArray(this->VAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// Fill EBO : Element Buffer objects with indexes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->size_of_indexes, this->indexes, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Circle::render() {
	glUseProgram(this->shader_program);
	//glUniform4f(glGetUniformLocation(this->shader_program, "colorInput"), 0.34f, 0.27f, 0.48f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glDrawElements(GL_TRIANGLES, (this->nb_segments * 3)-1, GL_UNSIGNED_INT, 0);

}
