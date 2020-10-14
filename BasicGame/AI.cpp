#include "AI.h"
#include "Gamestate.h"
#include "GameInput.h"

AI::AI()
{
	search_first_location = true;
}

void AI::SimulateInput(const Gamestate & gamestate, GameInput & input)
{
	// we have a goal, try and accomplish it
	input.lay_bomb = planned_create_bomb;
	input.motion = planned_target_direction;
}

void AI::Plan(Gamestate & gamestate)
{
	const Level & level = gamestate.level;
	const Player & ai_player = gamestate.players[player_id];

	// what is our current location
	current_location = ai_player.location;

	// which cell are we closest to
	nearest_cell = gamestate.level.NearestCell(current_location);

	if (search_first_location)
	{
		search_first_location = false;
		spawn_cell = nearest_cell;
	}




	const Player & local_player = gamestate.players[gamestate.local_player_id];
	glm::vec2 player_location = local_player.location;
	uint32_t player_cell = gamestate.level.NearestCell(player_location);


	bool evasive_manouvers = false;
	glm::ivec2 evasive_direction = glm::ivec2(0);

	glm::ivec2 cardinal_directions[4] = {
		glm::ivec2(1, 0),
		glm::ivec2(0, 1),
		glm::ivec2(-1, 0),
		glm::ivec2(0, -1)
	};
	std::vector<glm::ivec2> danger_directions;

	std::vector<CardinalSearchResult> cardinal_awareness = CardinalDirectionSearch(gamestate, nearest_cell, ai_player.explosion_radius);
	// if we are in the way of a pending bomb explosion, move out
	for (const auto & awareness : cardinal_awareness)
	{
		if (awareness.object_type == ObjectTypes::Bomb)
		{
			evasive_manouvers = true;
			danger_directions.push_back(awareness.direction);
		}
	}

	if (evasive_manouvers)
	{
		// just move, don't lay a bomb
		glm::ivec2 summed_danger(0);
		for (auto d : danger_directions)
		{
			summed_danger += d;
		}

		std::vector<glm::ivec2> potential_evasions;
		if (summed_danger.x == 0 && summed_danger.y == 0)
		{
			// we are either surrounded, or we have two polar directions
			if (danger_directions.size() == 4)
				// we are surrounded .: give up
				evasive_direction = glm::ivec2(0);
			else
			{
				// we have two opposite directions
				// pick a perpendicular path and go that direction
				potential_evasions.emplace_back(danger_directions[0].y, danger_directions[0].x);
				potential_evasions.emplace_back(-danger_directions[0].y, -danger_directions[0].x);
			}
		}
		else
		{
			// we have one, two, or three danger directions, move away
			switch (danger_directions.size())
			{
				default:
					break;
				case 1:
					// opposite, or two perpendiculars
					potential_evasions.emplace_back(glm::ivec2(-danger_directions[0].x, -danger_directions[0].y));
					potential_evasions.emplace_back(glm::ivec2(danger_directions[0].y, danger_directions[0].x));
					potential_evasions.emplace_back(glm::ivec2(-danger_directions[0].y, -danger_directions[0].x));
					break;
				case 2:
					// diagonal sum or directions touching
					potential_evasions.push_back(-danger_directions[0]);
					potential_evasions.push_back(-danger_directions[1]);
					break;
				case 3:
					// move the opposite of summed direction
					potential_evasions.push_back(-summed_danger);
					break;
			}

			
		}

		// for each potential evasion check if we can walk in that direction due to player or wall, bomb, etc.
		for (size_t i = 0; i < potential_evasions.size(); ++i)
		{
			glm::ivec2 pe = potential_evasions[i];
			// look in that direction for an open space
			CardinalSearchResult sres;
			// if there is nothing in that direction then evade there
			if (!CardinalDirectionSearchSingular(gamestate, nearest_cell, 1, pe, sres))
			{
				// use this one, good enough
				evasive_direction = pe;
				break;
			}
		}

		planned_target_direction = evasive_direction;
	}
	else
	{
		// we are currently safe

	}


	// BOMBS
	// bombs should be placed if:
	// - we would hit a destructible wall
	// - we would hit a path near the player (within x tiles of player)
	// - we are not in our spawn cell
	// we do not create bomb in our starting spot no matter what
	// this allows us to easily make the ai not guarantee kill itself everytime the game starts
	planned_create_bomb = !evasive_manouvers && nearest_cell != spawn_cell;

	// MOTION
	// for now pick one of 2 choices
	// move towards
	// move random
	// hold direction for 3-5 cells

	// what is player's position?
	// what is player's current trajectory?
	// try to get perpendicular to player/trajectory and close enough to place a bomb that can reach him
	// generate 3 points of the player
	// current
	// current if continuing trajectory
	// current if 

	// plan 3 seconds out
	// generate 7 points in a cross like shape
	// one for current position if the player doesn't move
	// one for current position + trajectory
	//planned_target_position = ;
}

bool AI::CardinalDirectionSearchSingular(const Gamestate & gamestate, uint32_t cell_index, size_t max_steps, glm::ivec2 direction, CardinalSearchResult & result) const
{
	const Level & level = gamestate.level;
	size_t tick_time = gamestate.game_time.tick_time;
	glm::ivec2 initial_cell = level.CellXY(cell_index);
	for (size_t i = 0; i < max_steps; ++i)
	{
		glm::ivec2 p = initial_cell + direction;
		uint32_t temp_cell_index = level.CellIndex(p);
		if (level.ValidCellIndex(temp_cell_index))
		{
			CellType cell_type = level.GetCellType(temp_cell_index);
			switch (cell_type)
			{
				case CellType::Floor:
					// check for bombs, explosions, players

					if (level.CellContainsExplosion(temp_cell_index, tick_time))
					{
						// cell contains explosion
						result = CardinalSearchResult(direction, 1, ObjectTypes::Explosion);
						return true;
					}
					else
					{
						size_t bomb_id = level.CellContainsBomb(temp_cell_index, tick_time);
						if (bomb_id != -1)
						{
							// cell contains bomb
							result = CardinalSearchResult(direction, i + 1, ObjectTypes::Bomb);
							return true;
						}
						else
						{
							// check for players
							for (const auto & p : gamestate.players)
							{
								uint32_t nearest_cell = level.NearestCell(p.location);
								if (nearest_cell == temp_cell_index)
								{
									result = CardinalSearchResult(direction, i + 1, ObjectTypes::Player);
									return true;
								}
							}
						}
					}
					break;
				case CellType::Wall_Destructable:
					result = CardinalSearchResult(direction, i + 1, ObjectTypes::Wall_Destructable);
					return true;
				case CellType::Wall_Static:
					result = CardinalSearchResult(direction, i + 1, ObjectTypes::Wall_Static);
					return true;
			}
		}

		p += direction;
	}
	return false;
}

std::vector<CardinalSearchResult> AI::CardinalDirectionSearch(const Gamestate & gamestate, uint32_t cell_index, size_t max_steps) const
{
	std::vector<CardinalSearchResult> result;
	const Level & level = gamestate.level;
	size_t tick_time = gamestate.game_time.tick_time;

	glm::ivec2 cardinal_increments[4] = {
		glm::ivec2(1, 0),
		glm::ivec2(0, 1),
		glm::ivec2(-1, 0),
		glm::ivec2(0, -1)
	};

	glm::ivec2 initial_cell = level.CellXY(cell_index);

	glm::ivec2 dir;
	uint32_t temp_cell_index;
	for (size_t direction_index = 0; direction_index < 4; ++direction_index)
	{
		dir = cardinal_increments[direction_index];
		glm::ivec2 p = initial_cell + dir;
		bool finished_searching_dir = false;
		for (size_t i = 0; i < max_steps; ++i)
		{
			temp_cell_index = level.CellIndex(p);
			if (level.ValidCellIndex(temp_cell_index))
			{
				CellType cell_type = level.GetCellType(temp_cell_index);
				switch (cell_type)
				{
					case CellType::Floor:
						// check for bombs, explosions, players

						if (level.CellContainsExplosion(temp_cell_index, tick_time))
						{
							// cell contains explosion
							result.emplace_back(CardinalSearchResult(dir, i + 1, ObjectTypes::Explosion));
							finished_searching_dir = true;
						}
						else
						{
							size_t bomb_id = level.CellContainsBomb(temp_cell_index, tick_time);
							if (bomb_id != -1)
							{
								// cell contains bomb
								result.emplace_back(CardinalSearchResult(dir, i + 1, ObjectTypes::Bomb));
								finished_searching_dir = true;
							}
							else
							{
								// check for players
								for (const auto & p : gamestate.players)
								{
									uint32_t nearest_cell = level.NearestCell(p.location);
									if (nearest_cell == temp_cell_index)
									{
										result.emplace_back(CardinalSearchResult(dir, i + 1, ObjectTypes::Player));
										finished_searching_dir = true;
									}
								}
							}
						}
						break;
					case CellType::Wall_Destructable:
						result.emplace_back(CardinalSearchResult(dir, i + 1, ObjectTypes::Wall_Destructable));
						finished_searching_dir = true;
						break;
					case CellType::Wall_Static:
						result.emplace_back(CardinalSearchResult(dir, i + 1, ObjectTypes::Wall_Static));
						finished_searching_dir = true;
						break;
				}
			}

			if (finished_searching_dir)
				break;

			p += dir;
		}
	}
	

	return result;
}
