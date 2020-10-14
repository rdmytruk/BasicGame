#pragma once
#include <glm/vec2.hpp>
// what are the player's actions
// these are processed from another input source into a relatively simple one
struct GameInput
{
	// which direction to move (axis aligned)
	glm::vec2 motion;
	bool lay_bomb;
};

