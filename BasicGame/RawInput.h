#pragma once
#include <Windows.h>
#include <vector>

struct GameInput;

// Raw input is what we are getting in from the computer/console/joystick/gamepad
// It can listen for many things but we really just want to know a few things
// such as button is pressed or not pressed for now.
struct RawInput
{
	bool keys_down[256];
	// flag to reset all down keys safely
	bool reset_keys;

	RawInput();

	// This is windows specific code
	// if we want linux or a different OS then conditionally compile here
	// or split to OS specific input classes
	// or split to input device specific input classes
	// Returns true if handled and lResult is set
	LRESULT MessagePump(
		HWND hWnd,			// Handle for this window
		UINT uMsg,			// Message for this window
		WPARAM wParam,		// Additional message information
		LPARAM lParam);		// Additional message information

	void ProcessFrame(GameInput & processed);
};

