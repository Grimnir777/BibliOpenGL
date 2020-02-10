#pragma once
#include "Shape.h"

class Arc : public Shape
{
public:
	Arc(
		unsigned int	shader_program,
		int				nb_segments,
		float			offsetAngle,
		float			angle,
		float			radius,
		float			prof,
		float			cx,
		float			cy);
	~Arc();
	void updateArc(
		int nb_segments,
		float offset_angle,
		float angle,
		float prof);
	void updateBuffers();
	void updateVertices();
	void render();
private:
	int nb_segments;
	float radius;
	float prof;
	float offset_angle;
	float angle;
	float* vertices;
	float* indexes;
	int size_of_vertices;
	int size_of_indexes;
};