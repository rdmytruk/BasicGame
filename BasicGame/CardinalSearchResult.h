#pragma once
#include <glm/vec2.hpp>
#include "ObjectTypes.h"
struct CardinalSearchResult
{
	glm::ivec2 direction;
	size_t distance;
	ObjectTypes object_type;

	CardinalSearchResult() {}
	CardinalSearchResult(glm::ivec2 direction, size_t distance, ObjectTypes type)
	{
		this->direction = direction;
		this->distance = distance;
		this->object_type = type;
	}
};

