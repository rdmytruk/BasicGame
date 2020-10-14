#pragma once
// Triggers are input sense and output data
// Does a trigger fire?
// If criteria is met, it will fire a trigger output
class Trigger
{
	// Fire trigger
	// what is our fire?
	
	// one case we can store a function pointer
	// and call it when we have met our criteria

	// or we can store criteria in a trigger
	// and return true/false when checked

	// what can we take in as data?
	// we need gamestate:
	//	- all player's input
	//  - level being played
	//  - explosions in existence
	//  - bombs in existence
	//  - tick time of game

	// Player positions
	// Level - cells, explosions
	

	// what triggers do we need in our program?
	// timer - after x time fire trigger
	//		used to wait for explosion and wait for explosion to end
	// position - if something at x position fire trigger
	// proximity - if something close to something fire trigger
	//		used for colliding with walls, players, bombs
	// count - number of X is some relationship to Y
	//		used for ending the game if one person or zero people are left
	//		used for limiting bombs
	// input - X button is pressed/released

	// what are trigger actions?
	// they can be functions that we call
	// or they can be other triggers that we create or remove

	// we can build our entire game loop just from triggers if so desired and store in a file to be loaded
	//	this could create a simple patching system where only non compiled code has to be changed
	// or we can use triggers as a rough concept and define all of these in written code
	//	this would create a more complex patching system where the exe will likely be replaced in a patch
};

