#pragma once
#include <stdint.h>
#include <vector>
#include <glm/vec2.hpp>
#include "CellType.h"
#include "Explosion.h"
#include "Bomb.h"
// a level contains the "board" and triggers
// different levels can provide unique gameplay

class Level
{
public:
	// how big are the cells?
	glm::vec2 cell_size;
	// cell count for the x and y direction
	glm::ivec2 cell_count;

	// we have a list of cells with different types
	
	// types:
	// wall
	// destructable wall
	// empty

	// explosions are tracked on a separate level

	std::vector<CellType> cells;
	std::vector<uint32_t> destructable_wall_cells;
	// we have raw explosion data in each cell saying there is an explosion until X time
	std::vector<Explosion> explosion_cells;
	std::vector<ActiveExplosion> active_explosions;
	// grid full of bomb lookup cells
	std::vector<BombLookup> bomb_cells;
	// list of all bombs in play
	std::vector<Bomb> bombs;
	size_t bomb_fuse_time;
	size_t explosion_persistence_time;
private:
	size_t next_bomb_id = 0;
public:
	void ClearBombs();
	void ClearExplosions();

	// attempts to add a bomb at the location
	bool AddBomb(Bomb & bomb, size_t tick_time);
	// cell_index - detonation point of the explosion
	// explosion_radius - how far the explosion is allowed to travel
	// begin_time - what tick time does it start at
	// active_time - what tick time will this explosion be valid until (inclusively)
	void AddExplosion(uint32_t cell_index, uint32_t explosion_radius, size_t begin_time, size_t active_time, uint8_t player_id);
	// checks if the cell has an explosion
	// cell_index - cell to check
	// tick_time - elapsed ticks in the game
	bool CellContainsExplosion(uint32_t cell_index, size_t tick_time) const;

	// check how many of these cells are valid and return the ones that are
	uint8_t ValidateCellIndices(std::vector<uint32_t> & cells, uint8_t count) const;
	uint8_t ConvertPositionToCellIndices(glm::vec2 position, std::vector<uint32_t> & cells) const;
	// Finds the nearest or best case cell that this position can be attributed to.
	// If a tie, choose bottom left most cell available
	uint32_t NearestCell(glm::vec2 position) const;
	bool NearestValidEmptyCell(glm::vec2 position, uint32_t & found_cell_index) const;

	glm::vec2 GetCellCenter(uint32_t cell_index) const;

	// converts from the x,y grid placement to index of the cell in an array
	uint32_t CellIndex(glm::ivec2 cell_xy) const;
	glm::ivec2 CellXY(uint32_t cell_index) const;
	bool ValidCellIndex(uint32_t cell_index) const;
	bool ValidCellXY(glm::ivec2 cell_xy) const;

	// checks if the cell contains a bomb, if so return bomb id
	size_t CellContainsBomb(uint32_t cell_index, size_t tick_time) const;

	CellType GetCellType(uint32_t cell_index) const;
};

