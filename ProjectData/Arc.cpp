#include "Arc.h"
#include "Shape.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <glad/glad.h>

Arc::Arc(
	unsigned int shader_program,
	int nb_segments,
	float offset_angle,
	float angle,
	float radius,
	float prof,
	float cx,
	float cy): Shape(shader_program)
{
	this->nb_segments =nb_segments,
	this->offset_angle = offset_angle,
	this->angle = angle,
	this->radius = radius,
	this->prof = prof,
	this->cx = cx;
	this->cy = cy;
	this->size_of_vertices = nb_segments * 4;
	this->size_of_indexes = nb_segments * 3;
	this->vertices = new float[this->size_of_vertices];
	this->indexes = new float[this->size_of_indexes];
}

Arc::~Arc()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Arc::updateArc(int nb_segments, float offset_angle, float angle, float prof) {
	this->nb_segments = nb_segments;
	this->offset_angle = offset_angle;
	this->angle = angle;
	this->prof = prof;
	this->updateVertices();
}

void Arc::updateVertices() {
	float radiusWithProf = this->radius + this->prof;
	float realOffset = 2.0f * 3.1415926f * this->offset_angle / 360;
	float realAngle = 2.0f * 3.1415926f * this->angle / 360;

	for (int i = 0; i < this->nb_segments; i++)
	{
		this->vertices[i * 4] = this->cx + (this->radius * cos((i * realAngle / this->nb_segments) + realOffset));
		this->vertices[(i * 4) + 1] = this->cy + (this->radius * sin((i * realAngle / this->nb_segments) + realOffset));

		this->vertices[(i * 4) + 2] = this->cx + (radiusWithProf * cos((i * realAngle / this->nb_segments) + realOffset));
		this->vertices[(i * 4) + 3] = this->cy + (radiusWithProf * sin((i * realAngle / this->nb_segments) + realOffset));

		std::cout << this->vertices[i * 4] << "," << this->vertices[(i * 4) + 1] << "\n";
		std::cout << this->vertices[(i * 4) + 2] << "," << this->vertices[(i * 4) + 3] << "\n";

		this->indexes[(i * 3)] = i;
		this->indexes[(i * 3) + 1] = i + 1;
		this->indexes[(i * 3) + 2] = i + 2;
	}
}

void Arc::updateBuffers()
{
	std::cout << "size of vertices" << this->size_of_vertices << "\n";
	std::cout << "size of indexes" << this->size_of_indexes << "\n";
	// Fill VBO : Vertex Buffer objects with vertices
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->size_of_vertices, vertices, GL_STATIC_DRAW);

	// Fill VAO : Vertex Array Object
	glBindVertexArray(this->VAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// Fill EBO : Element Buffer objects with indexes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->size_of_indexes, indexes, GL_STATIC_DRAW);

}

void Arc::render() {
	glUseProgram(this->shader_program);
	//glUniform4f(glGetUniformLocation(this->shader_program, "colorInput"), 0.34f, 0.27f, 0.48f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glDrawElements(GL_TRIANGLES, this->nb_segments * 5, GL_UNSIGNED_INT, 0);

}