#pragma once
#include <Windows.h>
#include "RawInput.h"

extern RawInput * raw_input;

namespace WindowsWrapper
{
	// https://docs.microsoft.com/en-us/windows/win32/winmsg/window-procedures
	LRESULT CALLBACK WndProc(
		HWND hWnd,		// Handle for this window
		UINT uMsg,		// Message for this window
		WPARAM wParam,	// Additional message information
		LPARAM lParam);	// Additional message information

	bool MessagePump();
};

