#pragma once
#include "Shape.h"

class Circle : public Shape
{
public:
	Circle(
		unsigned int shader_program,
		int nb_segments,
		float radius,
		float cx,
		float cy);
	~Circle();
	void updateVertices();
	void updateBuffers();
	void render();
	int nb_segments;
	float radius;
	float vertices[240];
	float indexes[360];
	int size_of_vertices;
	int size_of_indexes;
};