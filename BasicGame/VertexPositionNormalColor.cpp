#include "VertexPositionNormalColor.h"

VertexPositionNormalColor::VertexPositionNormalColor()
{
	triangle_count = 0;
	triangle_length = 0;
	position_vbo = 0;
	normal_vbo = 0;
	color_vbo = 0;
	vao = 0;
	create_buffers = false;
	shape_loaded = false;
}

VertexPositionNormalColor::VertexPositionNormalColor(VertexPositionNormalColor & copy)
{
	positions = copy.positions;
	normals = copy.normals;
	colors = copy.colors;
	triangle_count = copy.triangle_count;
	triangle_length = copy.triangle_length;
	position_vbo = copy.position_vbo;
	normal_vbo = copy.normal_vbo;
	color_vbo = copy.color_vbo;
	vao = copy.vao;
	create_buffers = copy.create_buffers;
	shape_loaded = copy.shape_loaded;
}

VertexPositionNormalColor & VertexPositionNormalColor::operator=(VertexPositionNormalColor copy)
{
	swap(*this, copy);
	return *this;
}

VertexPositionNormalColor::~VertexPositionNormalColor()
{
	// TODO free GPU memory
	// usually would glDeleteBuffers here to free up GPU memory
	// anything that is glCreated should be glDeleted
}

void VertexPositionNormalColor::LoadShape(const BasicModel & model)
{
	triangle_count = model.triangles.size();
	triangle_length = triangle_count * 3;

	GLuint triangles_added = 0;
	positions.reserve(triangle_length);
	normals.reserve(triangle_length);
	colors.reserve(triangle_length);
	for (GLuint i = 0; i < triangle_count; ++i)
	{
		glm::ivec3 t_indices = model.triangles[i];

		for (GLuint t = 0; t < 3; ++t)
		{
			positions.push_back(model.vertices[t_indices[t]].position);
			normals.push_back(model.vertices[t_indices[t]].normal);
			colors.push_back(model.vertices[t_indices[t]].color);
		}

		triangles_added++;
	}

	create_buffers = true;
	shape_loaded = true;
}

void VertexPositionNormalColor::Build()
{
	assert(shape_loaded && "Shape must be loaded to build a VertexPositionNormalColor");

	if (create_buffers)
	{
		CreateBuffers();
	}
	BuildVBOs();
	BuildVAO();
}

void VertexPositionNormalColor::Render() const
{
	assert(shape_loaded && "Shape must be loaded - VertexPositionNormalColor");
	assert(!create_buffers && "Need to create buffers - VertexPositionNormalColor");
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, triangle_length);
}

void VertexPositionNormalColor::CreateBuffers()
{
	glGenBuffers(1, &position_vbo);
	glGenBuffers(1, &normal_vbo);
	glGenBuffers(1, &color_vbo);
	// technically vertex array object, but we want it created once as well
	glGenVertexArrays(1, &vao);

	create_buffers = false;
}

void VertexPositionNormalColor::BuildVBOs()
{
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), colors.data(), GL_STATIC_DRAW);
}

void VertexPositionNormalColor::BuildVAO()
{
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
	// could set normalized to true here but we don't necessarily want it normalized for our hacked explosion objects for simplicity
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// unbind vao
	glBindVertexArray(0);
}

void swap(VertexPositionNormalColor & a, VertexPositionNormalColor & b)
{
	using std::swap;

	swap(a.positions, b.positions);
	swap(a.normals, b.normals);
	swap(a.colors, b.colors);
	swap(a.triangle_count, b.triangle_count);
	swap(a.triangle_length, b.triangle_length);
	swap(a.position_vbo, b.position_vbo);
	swap(a.normal_vbo, b.normal_vbo);
	swap(a.color_vbo, b.color_vbo);
	swap(a.vao, b.vao);
	swap(a.create_buffers, b.create_buffers);
	swap(a.shape_loaded, b.shape_loaded);
}
