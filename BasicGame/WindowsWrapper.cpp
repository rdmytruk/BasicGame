#include "WindowsWrapper.h"

// https://docs.microsoft.com/en-us/windows/win32/winmsg/window-procedures
LRESULT CALLBACK WindowsWrapper::WndProc(
	HWND hWnd,		// Handle for this window
	UINT uMsg,		// Message for this window
	WPARAM wParam,	// Additional message information
	LPARAM lParam)	// Additional message information
{
	switch (uMsg)
	{
		case WM_CREATE:
			// Initialize the window
			return 0;
		case WM_SIZE:
			return 0;
		case WM_SIZING:
			return 0;
		case WM_DESTROY:
		case WM_CLOSE:
		{
			// clean up window-specific data objects
			PostQuitMessage(0);
			return 0;
		}

		default:
			// attempt to handle input
			// otherwise return defwindowproc
			//return DefWindowProc(hWnd, uMsg, wParam, lParam);
			// we really want to pump into rawinput and see what we get
			return raw_input->MessagePump(hWnd, uMsg, wParam, lParam);
	}
}

bool WindowsWrapper::MessagePump()
{
	MSG msg;			// Windows message structure
	bool running = true;	// Bool variable to exit loop


	// read all windows messages before moving on
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			// if quit is received we don't want to keep pumping messages right now
			// we just want to quit
			running = false;
		}
		else
		{
			// Message pump to wndproc
			TranslateMessage(&msg);		// Translate the message
			DispatchMessage(&msg);		// Dispatch the message
		}
	}

	return running;
}

