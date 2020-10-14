#include "BasicModel.h"

BasicModel::BasicModel()
{
}

BasicModel::BasicModel(BasicModel & copy)
{
	vertices = copy.vertices;
	triangles = copy.triangles;
}

BasicModel & BasicModel::operator=(BasicModel copy)
{
	swap(*this, copy);
	return *this;
}

void swap(BasicModel & a, BasicModel & b)
{
	using std::swap;
	swap(a.vertices, b.vertices);
	swap(a.triangles, b.triangles);
}
