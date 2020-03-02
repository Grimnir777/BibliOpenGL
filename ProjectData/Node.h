#pragma once
#include <string>
#include <vector>
#include "glm.hpp"

class Node
{
public:
	Node()
	{
	}
	Node(glm::vec3 color, std::string value)
	{
		this->color = color;
		this->value = value;
	}
	Node(glm::vec3 color, std::string value, std::string count)
	{
		this->color = color;
		this->value = value;
		this->count = atoi(count.c_str());
	}
	~Node()
	{
	}
	void AddChild(Node* child)
	{
		child->parent = this;
		children.push_back(child);
	}
	glm::vec3 color;
	std::string value;
	int count = -1;

	bool IsLeaf() const
	{
		return (children.size() == 0) ? true : false;
	}

	bool IsRoot() const
	{
		return (parent == NULL) ? true : false;
	}

	Node* parent = NULL;
	std::vector<Node*> children;

};