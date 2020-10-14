#pragma once
#include <glm/vec3.hpp>

struct BombLookup
{
	// tick time to activate
	size_t activation_time;
	// bomb's unique id
	size_t bomb_id;
};

struct Bomb
{
	// what tick time should the detonation take place
	// also what time is the bomb active until (inclusive)
	size_t detonation_time;
	// unique id of the bomb
	size_t bomb_id;

	// who placed the bomb
	char player_id;

	// how large the explosion will be
	uint32_t blast_radius;

	// maybe instead of radius and blast radius
	// we can list modifiers instead that can change
	// maybe allow people to diffuse bombs

	//maybe false bombs can be placed
	// or smoke bombs so you can run over your own
	// then 2 players can fake each other out

	uint32_t cell_index;
};

