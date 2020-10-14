#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "Player.h"
#include "Bomb.h"
#include "Explosion.h"
#include "Level.h"
#include "VPNCRenderer.h"
#include "PlayerRenderer.h"
#include "VertexPositionNormalColor.h"

struct Gamestate;

// Game is our logic for the game
// we separated out the variables into a gamestate class
// this is done for ease of passing data around and trying to simplify/clean the function calls.
// Don't have to do this, but it gives us all the information to everything this way (good or bad)
// We can have the graphics sitting in game for now
class Game
{
private:
	const glm::vec3 static_floor_color = glm::vec3(0.87f);
	const glm::vec3 static_wall_color = glm::vec3(0.3f, 0.3f, 0.47f);
	const glm::vec3 destructable_wall_color = glm::vec3(0.6f);
	const glm::vec3 sky_blue = glm::vec3(0.573f, 0.886f, 0.992f);
	const glm::vec3 death_grey = glm::vec3(0.5f);
	const float floor_height = 0.01f;
	const float wall_height = 8.0f;

	glm::mat4 camera_world;
	glm::mat4 camera_view;
	glm::mat4 camera_projection;
	glm::mat4 camera_wvp;
	glm::vec3 light_direction;
	// render helper
	VPNCRenderer vpnc_renderer;
	PlayerRenderer player_renderer;
	// objects that will be rendered
	VertexPositionNormalColor vpnc_static;
	VertexPositionNormalColor vpnc_wall;
	VertexPositionNormalColor vpnc_player;
	VertexPositionNormalColor vpnc_explosion;
	VertexPositionNormalColor vpnc_bomb;
public:
	// need to start a game
	void Start(Gamestate & gamestate, size_t player_count);
	
	// loads the CPU
	// can be done in start
	void LoadCPU(Gamestate & gamestate);
	void GenerateLevel(Gamestate & gamestate);
	// loads the GPU
	// just needs to be done once for our game before it starts
	// split up the function calls of CPU and GPU in case we wanted to multithread later
	void LoadGPU(Gamestate & gamestate);

	// update a tick in our game
	bool Update(Gamestate & gamestate);

	// render the game
	void Render(Gamestate & gamestate) const;

private:
	void HandleInput(Gamestate & gamestate);
	void PerformPlayerActions(Gamestate & gamestate);
	void AISimulation(Gamestate & gamestate);
	void CheckBombsForDetonation(Gamestate & gamestate);
	void RemoveOldExplosions(Gamestate & gamestate);
	void CheckPlayersForDeath(Gamestate & gamestate);

	// creates cube vertices and triangles and appends them to model
	void AddCubeToModel(BasicModel & model, glm::vec3 cube_center, glm::vec3 cube_size, glm::vec3 color) const;

	void CreateStaticLevelGeometry(Gamestate & gamestate, BasicModel & model);
};

