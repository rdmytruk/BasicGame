#pragma once
#include <glm/vec2.hpp>
#include "GameTime.h"
#include "Level.h"
#include "GameInput.h"
#include "Player.h"
#include "AI.h"
#include "Bomb.h"
#include "Explosion.h"
//	- all player's input
//  - level being played
//  - explosions in existence
//  - bombs in existence
//  - tick time of game
struct Gamestate
{
	bool running;
	GameTime game_time;

	// how fast the player should move each frame
	float player_speed_scalar;

	// has map and derived explosion map
	Level level;

	// what is the id of our player in our list?
	int local_player_id;
	// we host a game with players
	std::vector<Player> players;
	// what ais are playing
	std::vector<AI> ais;

	std::vector<Explosion> explosions;

	GameInput input;

	glm::ivec2 screen_resolution;
};

