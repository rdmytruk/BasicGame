#include "Player.h"

Player::Player()
{
}

Player::Player(const Player & copy)
{
	player_id = copy.player_id;
	ai_id = copy.ai_id;
	capacity = copy.capacity;
	available = copy.available;
	cooldown_remaining = copy.cooldown_remaining;
	explosion_radius = copy.explosion_radius;
	location = copy.location;
	color = copy.color;
	alive = copy.alive;
	input = copy.input;
}

Player & Player::operator=(Player copy)
{
	swap(*this, copy);
	return *this;
}

void swap(Player & a, Player & b)
{
	using std::swap;
	swap(a.player_id, b.player_id);
	swap(a.ai_id, b.ai_id);
	swap(a.capacity, b.capacity);
	swap(a.available, b.available);
	swap(a.cooldown_remaining, b.cooldown_remaining);
	swap(a.explosion_radius, b.explosion_radius);
	swap(a.location, b.location);
	swap(a.color, b.color);
	swap(a.alive, b.alive);
	swap(a.input, b.input);
}
