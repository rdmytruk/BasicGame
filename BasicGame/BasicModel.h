#pragma once
#include <vector>
#include "BasicVertex.h"
struct BasicModel
{
	// vertex data of our basic model
	std::vector<BasicVertex> vertices;
	// each piece of the vector is an index into the vertices vector
	// .: each triangle is made of 3 vertices
	std::vector<glm::ivec3> triangles;

	BasicModel();
	BasicModel(BasicModel & copy);
	BasicModel & operator=(BasicModel copy);
	friend void swap(BasicModel & a, BasicModel & b);
};

