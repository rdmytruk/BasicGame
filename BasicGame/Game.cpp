#include "Game.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "Gamestate.h"
#include "BasicModel.h"

void Game::Start(Gamestate & gamestate, size_t player_count)
{
	gamestate.running = true;
	gamestate.game_time.tick_time = 0;
	gamestate.player_speed_scalar = 2.0f;
	gamestate.level.bomb_fuse_time = 240;
	gamestate.level.explosion_persistence_time = 50;

	glm::vec3 colors_available[] = {
		glm::vec3(0.4f, 1.0f, 0.4f),
		glm::vec3(1.0f, 0.4f, 0.6f),
		glm::vec3(0.4f, 0.4f, 1.0f),
		glm::vec3(0.2f, 0.6f, 0.8f),
		glm::vec3(0.8f, 0.6f, 0.2f),
		glm::vec3(0.2f, 0.8f, 0.6f),
		glm::vec3(0.8f, 0.2f, 0.6f),
		glm::vec3(0.6f, 0.2f, 0.8f),
		glm::vec3(0.6f, 0.8f, 0.2f),
	};

	size_t player_bomb_count = 3;
	size_t bomb_radius = 10;
	for (size_t i = 0; i < player_count; ++i)
	{
		Player p;

		p.alive = true;
		p.available = player_bomb_count;
		p.capacity = player_bomb_count;
		p.color = colors_available[i % colors_available->length()];
		p.cooldown_remaining = 0;
		p.explosion_radius = bomb_radius;
		p.location; // TODO find location for player to spawn
		p.player_id = i;

		if (i == 0)
		{
			gamestate.local_player_id = p.player_id;
			// the first player is our playable character
			p.ai_id = -1;
			// start our player at position 1,1
			glm::ivec2 start_cell(1, 1);
			p.location = gamestate.level.GetCellCenter(gamestate.level.CellIndex(start_cell));
		}
		else
		{
			// everything else is an ai
			AI ai;
			ai.player_id = p.player_id;
			p.ai_id = gamestate.ais.size();
			gamestate.ais.push_back(ai);

			glm::ivec2 start_cell(1, 9);
			p.location = gamestate.level.GetCellCenter(gamestate.level.CellIndex(start_cell));
		}

		gamestate.players.push_back(p);
	}
}

void Game::LoadCPU(Gamestate & gamestate)
{
	GenerateLevel(gamestate);

	glm::vec2 cell_size = gamestate.level.cell_size;
	glm::ivec2 cell_count = gamestate.level.cell_count;

	camera_world = glm::mat4(1.0f);

	// what size of the level is there
	glm::vec3 level_size(cell_size.x * cell_count.x, 50.0f, cell_size.y * cell_count.y);
	glm::vec3 level_center = level_size * 0.5f;
	float zoom_back = 200.0f;
	glm::vec3 eye_dir = glm::normalize(glm::vec3(level_center.x * 0.0f, level_center.y + 300.0f, level_center.z * -1.0f));
	glm::vec3 eye = level_center + zoom_back * eye_dir;
	glm::vec3 target = level_center;
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	camera_view = glm::lookAt(
		eye,
		target,
		up);
	float ortho_size = 100.0f;
	float near_point = 1.0f;
	float far_point = 3000.0f;
	float fov = glm::pi<float>() * 0.4f;
	float aspect_ratio = static_cast<float>(gamestate.screen_resolution.x) / static_cast<float>(gamestate.screen_resolution.y);
	camera_projection = glm::perspective(
		fov,
		aspect_ratio,
		near_point,
		far_point);
	camera_wvp = camera_projection * camera_view * camera_world;
	light_direction = glm::normalize(glm::vec3(1.0f, 1.5f, -0.5f));

	// Static level
	BasicModel static_model;
	CreateStaticLevelGeometry(gamestate, static_model);
	vpnc_static.LoadShape(static_model);

	// Destructable wall
	BasicModel destructable_cell_cube;
	glm::vec3 cube_size(cell_size.x, wall_height, cell_size.y);
	// want our cube origin to be at cell 0,0 in the center
	AddCubeToModel(destructable_cell_cube, cube_size * 0.5f, cube_size, destructable_wall_color);
	vpnc_wall.LoadShape(destructable_cell_cube);

	// Player
	BasicModel player_model;
	AddCubeToModel(player_model, cube_size * glm::vec3(0.0f, 0.5f, 0.0f), cube_size, glm::vec3(0.0f, 1.0f, 0.0f));
	vpnc_player.LoadShape(player_model);

	// Explosion
	BasicModel explosion_model;
	AddCubeToModel(explosion_model, cube_size * glm::vec3(0.5f, 0.5f, 0.5f), cube_size * glm::vec3(0.9f, 1.21f, 0.9f), glm::vec3(1.0f, 0.4f, 0.05f));
	vpnc_explosion.LoadShape(explosion_model);

	// Bomb
	BasicModel bomb_model;
	AddCubeToModel(bomb_model, cube_size * glm::vec3(0.5f, 0.5f, 0.5f), cube_size * glm::vec3(0.9f, 1.2f, 0.9f), glm::vec3(0.1f, 0.1f, 0.1f));
	vpnc_bomb.LoadShape(bomb_model);
}

void Game::GenerateLevel(Gamestate & gamestate)
{
	// load the level for our game
	Level & level = gamestate.level;
	level.cell_count = glm::ivec2(11, 11);
	level.cell_size = glm::vec2(20.0f, 20.0f);
	int total_cell_count = level.cell_count.x * level.cell_count.y;
	level.cells = std::vector<CellType>(total_cell_count);
	level.explosion_cells = std::vector<Explosion>(total_cell_count);
	level.ClearBombs();
	level.ClearExplosions();
	for (int y = 0; y < level.cell_count.y; ++y)
	{
		for (int x = 0; x < level.cell_count.x; ++x)
		{
			int cell_index = level.CellIndex(glm::ivec2(x, y));
			CellType ct;

			int x_wall_dist = std::min(x, (level.cell_count.x - 1) - x);
			int y_wall_dist = std::min(y, (level.cell_count.y - 1) - y);
			// if a border cell then static wall
			if (x_wall_dist == 0 ||
				y_wall_dist == 0)
			{
				ct = CellType::Wall_Static;
			}
			// we also want a nice grid of dotted static walls in between
			else if (x % 2 == 0 && y % 2 == 0)
			{
				ct = CellType::Wall_Static;
			}
			// make a patter of destructable walls
			else if (x_wall_dist > 2 || y_wall_dist > 2)
			{
				ct = CellType::Wall_Destructable;
				level.destructable_wall_cells.push_back(cell_index);
			}
			else
			{
				ct = CellType::Floor;
			}
			level.cells[cell_index] = ct;
		}
	}
}

void Game::CreateStaticLevelGeometry(Gamestate & gamestate, BasicModel & model)
{
	// our static gets a special model made up of cubes for each tile
	glm::ivec2 cell_count = gamestate.level.cell_count;
	glm::vec2 cell_size = gamestate.level.cell_size;
	glm::vec3 cube_center;
	glm::vec3 cube_size(cell_size.x, wall_height, cell_size.y);
	glm::vec3 cube_color;

	// we add one giant piece for the floor
	// then add static walls on top
	glm::vec3 level_size(cell_size.x * cell_count.x, 0.0f, cell_size.y * cell_count.y);
	AddCubeToModel(model, glm::vec3(level_size.x * 0.5f, 0.0f, level_size.z * 0.5f), level_size, static_floor_color);
	for (int y = 0; y < cell_count.y; ++y)
	{
		for (int x = 0; x < cell_count.x; ++x)
		{
			cube_center = glm::vec3(
				x * cell_size.x + cell_size.x * 0.5f,
				floor_height + cube_size.y * 0.5f,
				y * cell_size.y + cell_size.y * 0.5f);
			CellType cell_type = gamestate.level.GetCellType(gamestate.level.CellIndex(glm::ivec2(x, y)));
			if (cell_type != CellType::Wall_Static)
				continue;
			AddCubeToModel(model, cube_center, cube_size, static_wall_color);
		}
	}
}

void Game::LoadGPU(Gamestate & gamestate)
{
	vpnc_renderer.ReloadShaders();
	player_renderer.ReloadShaders();
	vpnc_static.Build();
	vpnc_wall.Build();
	vpnc_player.Build();
	vpnc_explosion.Build();
	vpnc_bomb.Build();
}

bool Game::Update(Gamestate & gamestate)
{
	// increase gametime via ticks
	gamestate.game_time.tick_time++;

	// perform ai planning
	AISimulation(gamestate);

	// should we be changing a bomb into an explosion
	CheckBombsForDetonation(gamestate);

	// any old inactive explosion animations we should remove
	RemoveOldExplosions(gamestate);

	// is the player dying right now?
	CheckPlayersForDeath(gamestate);

	// handle input from each player
	HandleInput(gamestate);

	// try to perform the input
	// if we are walking into a wall, the game should stop us
	PerformPlayerActions(gamestate);

	return gamestate.running;
}

void Game::HandleInput(Gamestate & gamestate)
{
	// for each player we handle input
	// - if real player then use computer input or network input
	// - if an AI then use an AI simulated input
	// this means we should have some sort of mapping between players and an input interface


	// then handle the result of everyone's input

	for (auto & p : gamestate.players)
	{
		if (!p.alive)
			continue;

		// Create input
		if (p.player_id == gamestate.local_player_id)
		{
			// player input
			// Apply input to our player
			gamestate.players[gamestate.local_player_id].input = gamestate.input;
		}
		else if (p.ai_id != -1)
		{
			// if player is an AI
			// do AI stuff here to simulate an input
			gamestate.ais[p.ai_id].SimulateInput(gamestate, gamestate.players[p.player_id].input);
		}
	}
}

void Game::PerformPlayerActions(Gamestate & gamestate)
{
	for (auto & p : gamestate.players)
	{
		// try to enact input
		if (!p.alive)
			continue;

		// Try to lay bombs
		if (p.input.lay_bomb &&
			p.capacity > 0)
		{
			uint32_t bomb_cell;
			bool available_space = gamestate.level.NearestValidEmptyCell(p.location, bomb_cell);
			if (available_space)
			{
				Bomb b;
				b.player_id = p.player_id;
				b.blast_radius = p.explosion_radius;
				b.cell_index = bomb_cell;
				b.detonation_time = gamestate.game_time.tick_time + gamestate.level.bomb_fuse_time;
				glm::vec2 location = gamestate.level.GetCellCenter(bomb_cell);

				if (gamestate.level.AddBomb(b, gamestate.game_time.tick_time))
				{
					p.capacity--;
				}
			}
		}
		// try to move
		// if we hit a wall, send feedback to the player
		p.location += p.input.motion * gamestate.player_speed_scalar;
	}
}

void Game::AISimulation(Gamestate & gamestate)
{
	// each ai creates a plan
	// then input is simulated to enact that plan
	for (auto & ai : gamestate.ais)
	{
		if (gamestate.players[ai.player_id].alive)
			ai.Plan(gamestate);
	}
}

void Game::CheckBombsForDetonation(Gamestate & gamestate)
{
	size_t tick_time = gamestate.game_time.tick_time;
	Level & level = gamestate.level;
	std::vector<int> marked_deletion_bombs;

	// iterate through the bombs and check if we can perform an explosion
	for (auto & b : gamestate.level.bombs)
	{
		// we only detonate on the frame itself
		// that way we don't have to update explosive cells more than adding new ones
		if (b.detonation_time == tick_time ||
			level.CellContainsExplosion(b.cell_index, tick_time))
		{
			// erase the bomb
			marked_deletion_bombs.push_back(b.bomb_id);

			// whichever player created the bomb gets +1 capacity
			gamestate.players[b.player_id].capacity++;

			// add an explosion
			level.AddExplosion(b.cell_index, b.blast_radius, tick_time, tick_time + level.explosion_persistence_time, b.player_id);
		}
	}

	if (marked_deletion_bombs.size() > 0)
	{
		for (size_t i = 0; i < marked_deletion_bombs.size(); ++i)
		{
			size_t id = marked_deletion_bombs[i];
			for (size_t b = 0; b < level.bombs.size(); ++b)
			{
				Bomb & bomb = level.bombs[b];
				if (bomb.bomb_id == id)
				{
					level.bombs.erase(level.bombs.begin() + b);
					break;
				}
			}
		}
	}

	// is there anything crossing an explosion?
	// explosions are cardinal directed and not area growth
	// so explosions travel from point of detonation outwards purely on the x-axis explicitly and y-axis explicitly

}

void Game::RemoveOldExplosions(Gamestate & gamestate)
{
	size_t tick_time = gamestate.game_time.tick_time;
	Level & level = gamestate.level;
	size_t i;
	for (i = 0; i < level.active_explosions.size(); ++i)
	{
		if (tick_time < level.active_explosions[i].end_time)
		{
			break;
		}
	}
	// i gives us the first index where we are still in time
	if (i > 0)
	{
		level.active_explosions.erase(level.active_explosions.begin(), level.active_explosions.begin() + i);
	}
}

void Game::CheckPlayersForDeath(Gamestate & gamestate)
{
	Level & level = gamestate.level;

	// a player can be on 1-4 differnt tiles
	// find out which tiles each player is on
	// check those tiles to see if they are marked as having an explosion

	if (level.active_explosions.size() == 0)
		return; // no explosions = no death for now

	std::vector<uint32_t> cell_buffer = std::vector<uint32_t>(4);
	for (auto & p : gamestate.players)
	{
		if (!p.alive)
			continue;

		uint8_t cells =	level.ConvertPositionToCellIndices(p.location, cell_buffer);
		cells = level.ValidateCellIndices(cell_buffer, cells);
		// iterate through each cell and look for an explosion to kill the player
		for (uint8_t i = 0; i < cells; ++i)
		{
			uint32_t cell_index = cell_buffer[i];
			// we want to check valid cells and only floor cells, not walls we are brushing up against
			if (level.GetCellType(cell_index) == CellType::Floor &&
				level.CellContainsExplosion(cell_index, gamestate.game_time.tick_time))
			{
				p.alive = false;
				break;
			}
		}
	}
}

void Game::AddCubeToModel(BasicModel & model, glm::vec3 cube_center, glm::vec3 cube_size, glm::vec3 color) const
{
	// we want to add 
	// 4 points per side due to wanting non interpolated normals (4*6 = 24)
	// 2 triangles per side (2*6 = 12)

	// first get the first index of the vertices to be used for triangle data
	size_t base_index = model.vertices.size();
	// bt is used just for a helper variable to make thinking about vertex indices easier be subdividing the problem into faces
	glm::ivec3 bt(base_index);
	// add 4 each time due to number of vertices added per cube face (4 = 2 triangles per face)
	glm::ivec3 bt_incr(4);

	glm::vec3 half_size = cube_size * 0.5f;
	
	auto & vertices = model.vertices;
	auto & triangles = model.triangles;

	glm::vec3 n_up(0.0f, 1.0f, 0.0f);
	glm::vec3 n_down(0.0f, -1.0f, 0.0f);
	glm::vec3 n_right(1.0f, 0.0f, 0.0f);
	glm::vec3 n_left(-1.0f, 0.0f, 0.0f);
	glm::vec3 n_front(0.0f, 0.0f, 1.0f);
	glm::vec3 n_back(0.0f, 0.0f, -1.0f);

	// order 'N' shape vertices in CW order
	// can define CCW instead if wanted; just need to know which when we set up the draw call for culling

	// TOP
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_up + half_size * (n_left + n_front), n_up, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_up + half_size * (n_left + n_back), n_up, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_up + half_size * (n_right + n_front), n_up, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_up + half_size * (n_right + n_back), n_up, color));
	triangles.push_back(bt + glm::ivec3(0, 1, 2));
	triangles.push_back(bt + glm::ivec3(2, 1, 3));
	bt += bt_incr;

	// BOT
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_down + half_size * (n_left + n_front), n_down, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_down + half_size * (n_left + n_back), n_down, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_down + half_size * (n_right + n_front), n_down, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_down + half_size * (n_right + n_back), n_down, color));
	triangles.push_back(bt + glm::ivec3(2, 1, 0));
	triangles.push_back(bt + glm::ivec3(3, 1, 2));
	bt += bt_incr;

	// FRONT
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_front + half_size * (n_left + n_up), n_front, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_front + half_size * (n_left + n_down), n_front, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_front + half_size * (n_right + n_up), n_front, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_front + half_size * (n_right + n_down), n_front, color));
	triangles.push_back(bt + glm::ivec3(2, 1, 0));
	triangles.push_back(bt + glm::ivec3(3, 1, 2));
	bt += bt_incr;

	// BACK
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_back + half_size * (n_left + n_up), n_back, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_back + half_size * (n_left + n_down), n_back, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_back + half_size * (n_right + n_up), n_back, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_back + half_size * (n_right + n_down), n_back, color));
	triangles.push_back(bt + glm::ivec3(0, 1, 2));
	triangles.push_back(bt + glm::ivec3(2, 1, 3));
	bt += bt_incr;

	// LEFT
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_left + half_size * (n_up + n_front), n_left, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_left + half_size * (n_up + n_back), n_left, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_left + half_size * (n_down + n_front), n_left, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_left + half_size * (n_down + n_back), n_left, color));
	triangles.push_back(bt + glm::ivec3(2, 1, 0));
	triangles.push_back(bt + glm::ivec3(3, 1, 2));
	bt += bt_incr;

	// RIGHT
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_right + half_size * (n_up + n_front), n_right, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_right + half_size * (n_up + n_back), n_right, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_right + half_size * (n_down + n_front), n_right, color));
	vertices.emplace_back(BasicVertex(cube_center + half_size * n_right + half_size * (n_down + n_back), n_right, color));
	triangles.push_back(bt + glm::ivec3(0, 1, 2));
	triangles.push_back(bt + glm::ivec3(2, 1, 3));
	bt += bt_incr;
}

void Game::Render(Gamestate & gamestate) const
{
	// perform rendering of the game here
	// place no game logic in this area
	// other than detecting what needs to be drawn


	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);


	// let's clear a nice sky blue when alive, grey when dead
	float player_alive = 1.0f * gamestate.players[gamestate.local_player_id].alive;
	glm::vec3 clear_color = glm::mix(death_grey, sky_blue, player_alive);
	glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// this is by no means efficient
	// efficiency is using instancing and other nice batching methods to reduce draw/render calls
	// less calls to GPU = generally better

	// what is our camera data?
	vpnc_renderer.UseProgram();
	vpnc_renderer.SetCamera(camera_wvp);
	vpnc_renderer.SetLightDirection(light_direction);

	// render the floor/static objects with no offset
	vpnc_renderer.SetPosition(glm::vec3(0.0f));
	vpnc_static.Render();

	Level & level = gamestate.level;
	glm::vec2 cell_size = level.cell_size;
	// TODO render the explosions
	for (size_t i = 0; i < level.active_explosions.size(); ++i)
	{
		ActiveExplosion & ae = level.active_explosions[i];
		glm::ivec2 xy = level.CellXY(ae.cell_index);
		vpnc_renderer.SetPosition(glm::vec3(xy.x * cell_size.x, 0.0f, xy.y * cell_size.y));
		vpnc_explosion.Render();
	}

	// render the destructable walls
	for (uint32_t index : level.destructable_wall_cells)
	{
		glm::ivec2 xy = level.CellXY(index);
		vpnc_renderer.SetPosition(glm::vec3(xy.x * cell_size.x, 0.0f, xy.y * cell_size.y));
		vpnc_wall.Render();
	}


	// Bombs
	for (size_t i = 0; i < gamestate.level.bombs.size(); ++i)
	{
		glm::ivec2 xy = level.CellXY(gamestate.level.bombs[i].cell_index);
		vpnc_renderer.SetPosition(glm::vec3(xy.x * cell_size.x, 0.0f, xy.y * cell_size.y));
		vpnc_bomb.Render();
	}

	player_renderer.UseProgram();
	player_renderer.SetCamera(camera_wvp);
	player_renderer.SetLightDirection(light_direction);
	// render all players
	// TODO new shader for players VPN c override so that we can have player specific colors
	for (const auto & p : gamestate.players)
	{
		player_renderer.SetColor(p.color);
		if (p.alive)
		{
			player_renderer.SetPosition(glm::vec3(p.location.x, 0.0f, p.location.y));
		}
		else
		{
			// place players in order outside level
			player_renderer.SetPosition(glm::vec3(0.0f + p.player_id * level.cell_size.x * 1.1f, 0.0f, 0.0f + level.cell_size.y * (level.cell_count.y + 1.1f)));
		}
		vpnc_player.Render();
	}

}
