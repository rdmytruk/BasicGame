#pragma once
#include <stdint.h>
struct Explosion
{
	char player_id;
	// what time is this active until
	size_t active_time;
};

struct ActiveExplosion
{
	// what time the explosion started and ended
	size_t begin_time;
	size_t end_time;
	uint32_t cell_index;

	ActiveExplosion() {}
	ActiveExplosion(size_t begin_time, size_t end_time, uint32_t cell_index)
	{
		this->begin_time = begin_time;
		this->end_time = end_time;
		this->cell_index = cell_index;
	}
};