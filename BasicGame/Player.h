#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "GameInput.h"
struct Player
{
	size_t player_id;

	// id of our ai, or -1 if no ai
	int ai_id;


	// number of bombs allowed at once
	char capacity;

	// number of unused bombs available to use
	char available;

	// how much longer we must wait until we can place another bomb
	char cooldown_remaining;

	// how much power our bombs have
	size_t explosion_radius;

	// location of the player on the map
	glm::vec2 location;

	// player color
	glm::vec3 color;

	bool alive;

	// NEED PLAYER INPUT
	// - can be real input
	// - can be internet input
	// - can be ai input
	GameInput input;

	// bombs placed:
	// player-id of who placed it
	// tick time of when placed
	// power range of how far the explosion is
	// - when explodes add 1 to available counter

	Player();
	Player(const Player & copy);
	Player & operator=(Player copy);
	friend void swap(Player & a, Player & b);
};

