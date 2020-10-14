#pragma once
#include <glm/vec3.hpp>
struct BasicVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;

	BasicVertex() {}
	BasicVertex(glm::vec3 position, glm::vec3 normal, glm::vec3 color)
	{
		this->position = position;
		this->normal = normal;
		this->color = color;
	}
};

