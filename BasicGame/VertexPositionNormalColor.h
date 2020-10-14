#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include "BasicModel.h"
// object containing our GPU data and data sender
class VertexPositionNormalColor
{
public:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colors;

	GLuint triangle_count;
	GLuint triangle_length;

	GLuint position_vbo;
	GLuint normal_vbo;
	GLuint color_vbo;
	GLuint vao;

	bool create_buffers;
	bool shape_loaded;

public:

	VertexPositionNormalColor();
	VertexPositionNormalColor(VertexPositionNormalColor & copy);
	VertexPositionNormalColor & operator=(VertexPositionNormalColor copy);
	friend void swap(VertexPositionNormalColor & a, VertexPositionNormalColor & b);

	~VertexPositionNormalColor();

	void LoadShape(const BasicModel & model);
	// moves the data from the CPU to the GPU
	// needs to be done on the gpu creation thread
	void Build();
	// tells the GPU to pass the data into the loaded shader
	// needs to be done on the gpu creation thread
	void Render() const;

private:
	void CreateBuffers();
	void BuildVBOs();
	void BuildVAO();

};

