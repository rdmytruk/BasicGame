#include "Level.h"
#include <glm/geometric.hpp>

void Level::ClearBombs()
{
	bombs.clear();

	size_t cell_total = cell_count.x * cell_count.y;
	bomb_cells = std::vector<BombLookup>(cell_total);
	for (size_t i = 0; i < cell_total; ++i)
	{
		bomb_cells[i].activation_time = 0;
		bomb_cells[i].bomb_id = -1;
	}
}

void Level::ClearExplosions()
{
	size_t cell_total = cell_count.x * cell_count.y;
	explosion_cells = std::vector<Explosion>(cell_total);
	for (size_t i = 0; i < cell_total; ++i)
	{
		explosion_cells[i].active_time = 0;
		explosion_cells[i].player_id = -1;
	}
}

bool Level::AddBomb(Bomb & bomb, size_t tick_time)
{
	// attempt to place a bomb at the location
	// is the cell free?
	bool cell_is_floor = (GetCellType(bomb.cell_index) == CellType::Floor);
	bool cell_has_no_bombs = CellContainsBomb(bomb.cell_index, tick_time) == -1;
	bool added = cell_is_floor && cell_has_no_bombs;

	if (added)
	{
		bomb.bomb_id = next_bomb_id++;
		bombs.push_back(bomb);
		bomb_cells[bomb.cell_index].bomb_id = bomb.bomb_id;
		bomb_cells[bomb.cell_index].activation_time = bomb.detonation_time;
	}

	return added;
}

void Level::AddExplosion(uint32_t cell_index, uint32_t explosion_radius, size_t begin_time, size_t active_time, uint8_t player_id)
{
	glm::ivec2 cardinal_increments[4] = {
		glm::ivec2(1, 0),
		glm::ivec2(0, 1),
		glm::ivec2(-1, 0),
		glm::ivec2(0, -1)
	};

	// we paint an explosion grid with explosion active tine
	// we place an explosion at this point and the others within radius

	// iterate from center outwards in 4 cardinal directions
	glm::ivec2 center = CellXY(cell_index);
	// move leftwards
	glm::ivec2 p;
	glm::ivec2 increment;
	bool finished_explosion;

	// add explosion on the center cell
	explosion_cells[cell_index].active_time = active_time;
	explosion_cells[cell_index].player_id = player_id;
	active_explosions.emplace_back(ActiveExplosion(begin_time, active_time, cell_index));

	for (size_t c = 0; c < 4; ++c)
	{
		increment = cardinal_increments[c];
		p = center + increment;
		finished_explosion = false;
		for (uint32_t i = 1; i < explosion_radius; ++i)
		{
			cell_index = CellIndex(p);
			if (ValidCellIndex(cell_index))
			{
				bool add_explosion_to_cell = true;
				switch (GetCellType(cell_index))
				{
					default:
					case CellType::Floor:
						// this is fine, continue on
						break;
					case CellType::Wall_Destructable:
						// this is the last explosion created
						finished_explosion = true;
						// need to destroy wall here
						// simple loop for now
						for (size_t q = 0; q < destructable_wall_cells.size(); ++q)
						{
							uint32_t index = destructable_wall_cells[q];
							if (index == cell_index)
							{
								destructable_wall_cells.erase(destructable_wall_cells.begin() + q);
								cells[cell_index] = CellType::Floor;
								break;
							}
						}
						break;
					case CellType::Wall_Static:
						// this is the last explosion created
						add_explosion_to_cell = false;
						finished_explosion = true;
						break;
				}
				if (add_explosion_to_cell)
				{
					explosion_cells[cell_index].active_time = active_time;
					explosion_cells[cell_index].player_id = player_id;
					active_explosions.emplace_back(ActiveExplosion(begin_time, active_time, cell_index));
				}
			}
			else
			{
				finished_explosion = true;
			}

			p += increment;

			if (finished_explosion)
				break;
		}
	}
}

bool Level::CellContainsExplosion(uint32_t cell_index, size_t tick_time) const
{
	return ValidCellIndex(cell_index) && explosion_cells[cell_index].active_time >= tick_time;
}

uint8_t Level::ValidateCellIndices(std::vector<uint32_t>& cells, uint8_t count) const
{
	uint8_t valid = 0;
	for (uint8_t i = 0; i < count; ++i)
	{
		if (ValidCellIndex(cells[i]))
		{
			// write the cell as valid to the front
			// so we can access vector in order still if some of the front ones are invalid
			cells[valid++] = cells[i];
		}
	}
	return valid;
}

uint8_t Level::ConvertPositionToCellIndices(glm::vec2 position, std::vector<uint32_t>& cells) const
{
	const float movement_axis_threshold = 0.01f;
	const float movement_radius_sq_threshold = movement_axis_threshold * movement_axis_threshold;

	// we assume the position/player is at most the size of one cell
	// therefore we return anywhere from 1 to 4 cells


	// our cell in fractional terms
	// location = cell_size * (xy + 0.5)
	// xy = (location / cell_size) - 0.5
	glm::vec2 approx_cell = (position / cell_size) - glm::vec2(0.5f);
	glm::ivec2 cell(approx_cell);

	// assume the origin of the map is 0,0
	// what is the cell center?
	glm::vec2 cell_center = GetCellCenter(CellIndex(cell));

	glm::vec2 remainder = position - cell_center;
	// signbit is 0 when positive 1 when negative
	// *2 - 1 = -1, 1
	// *-2 + 1 = 1, -1
	int x_offset = std::signbit(remainder.x) * -2 + 1;
	int y_offset = std::signbit(remainder.y) * -2 + 1;

	// if there is a very small remainder then we classify it as fully within the one cell
	// a dot product of a vector of itself is square distance
	if (glm::dot(remainder, remainder) < movement_radius_sq_threshold)
	{
		cells[0] = CellIndex(cell);
		return 1;
	}


	// we need to check x and y individually for being within a constraint
	if (abs(remainder.x) < movement_axis_threshold)
	{
		// we have a negligible amount of x traversal
		// .: we assume purely y axis traversal
		cells[0] = CellIndex(cell);
		cells[1] = CellIndex(glm::ivec2(cell.x, cell.y + y_offset));
		return 2;
	}

	if (abs(remainder.y) < 0.01f)
	{
		// we have a negligible amount of y traversal
		// .: we assume purely x axis traversal
		cells[0] = CellIndex(cell);
		cells[1] = CellIndex(glm::ivec2(cell.x + x_offset, cell.y));
		return 2;
	}

	// we are too far from center in either direction
	// .: we assume we are in 4 different cells
	cells[0] = CellIndex(cell);
	cells[1] = CellIndex(glm::ivec2(cell.x + x_offset, cell.y));
	cells[1] = CellIndex(glm::ivec2(cell.x, cell.y + y_offset));
	cells[2] = CellIndex(glm::ivec2(cell.x + x_offset, cell.y + y_offset));
	return 4;
}

uint32_t Level::NearestCell(glm::vec2 position) const
{
	glm::vec2 approx_cell = (position / cell_size) - glm::vec2(0.5f);
	glm::ivec2 cell(approx_cell);
	return CellIndex(cell);
}

bool Level::NearestValidEmptyCell(glm::vec2 position, uint32_t & found_cell_index) const
{
	std::vector<uint32_t> cell_buffer(4);
	uint8_t cell_count = ConvertPositionToCellIndices(position, cell_buffer);
	cell_count = ValidateCellIndices(cell_buffer, cell_count);
	float min_dist_sq = FLT_MAX;
	bool found_valid_space = false;
	// pick the nearest cell index remaining that has free space available
	for (uint8_t i = 0; i < cell_count; ++i)
	{
		uint32_t cell_index = cell_buffer[i];
		CellType ct = GetCellType(cell_index);
		if (ct == CellType::Floor)
		{
			glm::vec2 center = GetCellCenter(cell_index);
			glm::vec2 diff = center - position;
			float sq_dist = glm::dot(diff, diff);
			if (sq_dist < min_dist_sq)
			{
				min_dist_sq = sq_dist;
				found_cell_index = cell_index;
				found_valid_space = true;
			}
		}
	}
	return found_valid_space;
}

glm::vec2 Level::GetCellCenter(uint32_t cell_index) const
{
	glm::ivec2 xy = CellXY(cell_index);
	return glm::vec2(cell_size.x * (xy.x + 0.5f), cell_size.y * (xy.y + 0.5f));
}

uint32_t Level::CellIndex(glm::ivec2 cell_xy) const
{
	// x + y * width
	// it is just an arbitrary convention, can be y + x * height if we wanted
	// or any other way of neatly using a solid block of memory
	// as long as it is consistent
	return cell_xy.x + cell_xy.y * cell_count.x;
}

glm::ivec2 Level::CellXY(uint32_t cell_index) const
{
	return glm::ivec2(cell_index % cell_count.x, cell_index / cell_count.x);
}

bool Level::ValidCellIndex(uint32_t cell_index) const
{
	// due to being an unsigned int we just need to check if < max value
	// as negative values will wrap around to an extreme positive
	return cell_index < cell_count.x * cell_count.y;
}

bool Level::ValidCellXY(glm::ivec2 cell_xy) const
{
	// our map is 0,0 to cell_count.x,cell_count.y
	return
		cell_xy.x >= 0 && cell_xy.x <= cell_count.x &&
		cell_xy.y >= 0 && cell_xy.y <= cell_count.y;
}

size_t Level::CellContainsBomb(uint32_t cell_index, size_t tick_time) const
{
	if (tick_time <= bomb_cells[cell_index].activation_time)
	{
		return bomb_cells[cell_index].bomb_id;
	}

	return -1;
}

CellType Level::GetCellType(uint32_t cell_index) const
{
	return cells[cell_index];
}
