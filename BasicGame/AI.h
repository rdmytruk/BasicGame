#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include "ObjectTypes.h"
#include "CardinalSearchResult.h"

struct Gamestate;
struct GameInput;

// We will create a simple AI
// it knows where it has been, and what it has seen
// it will attempt to avoid bombs
class AI
{
public:
	// which player are we controlling
	char player_id;
private:
	// heatmap for where we are and have been
	// we want to not stay in one spot so we will be incentivized to move around
	// do some randomization so we either move out of heat, or random direction
	std::vector<float> self_heatmap;

	// which cell are we closest to
	uint32_t nearest_cell;

	// what is our current location
	glm::vec2 current_location;
	// move in target direction
	glm::vec2 planned_target_direction;
	// attempt to place bomb
	bool planned_create_bomb;
	// how many cells have we traveled since our last plan
	int cells_traveled_since_plan;

	
	// we can do another heatmap for target priorities
	// super hot for enemies near
	// mildly warm for destructable blocks
	// - ai will target hottest target x% of the time
	// - ai will lay random bombs 100-x% of the time

	// if more than 2 enemies left do bombing patterns
	// - chain explosions
	// - constant bombardment
	// if 1v1
	// we will track where the player is and try to guess his movements
	// we can place a bomb on his moving path or we can attack where he came from

	// we want to know our first location
	bool search_first_location;
	uint32_t spawn_cell;
public:
	AI();

	void SimulateInput(const Gamestate & gamestate, GameInput & input);

	// Attempt to create a plan for what we shall do
	void Plan(Gamestate & gamestate);

private:
	bool CardinalDirectionSearchSingular(const Gamestate & gamestate, uint32_t cell_index, size_t max_steps, glm::ivec2 direction, CardinalSearchResult & result) const;
	// search in 4 directions from a cell
	// looking for bombs, players, destructable walls, static walls
	// cell_index - index to start search from
	// max_steps - maximum amount of steps in each direction
	std::vector<CardinalSearchResult> CardinalDirectionSearch(const Gamestate & gamestate, uint32_t cell_index, size_t max_steps) const;
};

