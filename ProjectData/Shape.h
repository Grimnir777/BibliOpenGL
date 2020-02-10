#pragma once

class Shape
{
public:
	Shape(unsigned int shader_program);
	float cx;
	float cy;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	unsigned int shader_program;
};