#include "Main.h"

// BasicGameDemo - created by Robert Dmytruk
// An example to show how a game and game engine works
// A grid game in 3D of players, walls, and bombs

// You can reduce the size of the Windows header files by excluding some of the less common API declarations as follows:
// Define WIN32_LEAN_AND_MEAN to exclude APIs such as Cryptography, DDE, RPC, Shell, and Windows Sockets.
//https://docs.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <tchar.h>
#endif

#include "Game.h"
#include "Gamestate.h"
#include "RawInput.h"
#include "RenderContext.h"
#include "WindowsWrapper.h"

RawInput * raw_input;

// You can reduce the size of the Windows header files by excluding some of the less common API declarations as follows:
// Define WIN32_LEAN_AND_MEAN to exclude APIs such as Cryptography, DDE, RPC, Shell, and Windows Sockets.
//https://docs.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers
#ifdef _WIN32
// Entry point for Windows Application
int WINAPI WinMain(HINSTANCE hInstance,		// Instance
	HINSTANCE hPrevInstance,				// Previous instance
	LPSTR lpCmdLine,						// Command line parameters
	int nCmdShow)							// Window show state
{
	size_t num_players = 2;
	glm::ivec2 window_position(50, 50);
	glm::ivec2 window_resolution(1280, 720);

	// INIT
	RenderContext render_context;
	render_context.CreateApplicationWindow(window_position, window_resolution);
	render_context.CreateGLWindow();
	render_context.ShowApplicationWindow();
	render_context.SetVSync(true);


	Game game;
	Gamestate gamestate;
	RawInput input;
	raw_input = &input;
	gamestate.screen_resolution = window_resolution;
	

	game.LoadCPU(gamestate);
	game.LoadGPU(gamestate);
	game.Start(gamestate, num_players);
	bool running = true;

	// GAME LOOP
	while (running)
	{
		// process raw input separately so our game class doesn't have much/any windows specific code in it
		input.ProcessFrame(gamestate.input);
		running &= game.Update(gamestate);
		game.Render(gamestate);

		// perform a buffer swap
		// we are rendering into a back buffer (hidden) then swapping it to front (visible) to minimize screen tearing
		render_context.SwapBuffer();

		// perform the message pumping of a windows application
		// reads different messages about the window being moved, resized, closed, input coming in etc.
		// when these values are translated they are sent to wndproc in WindowsWrapper::WndProc
		// if we do not perform the message pump, our application looks like it freezes/acts unresponsive
		running &= WindowsWrapper::MessagePump();
	}

	return 0;
}
#endif
