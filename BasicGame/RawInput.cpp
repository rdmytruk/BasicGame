#include "RawInput.h"
#include "GameInput.h"

RawInput::RawInput()
{
	reset_keys = true;
}

LRESULT RawInput::MessagePump(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// other useful documentation for windows keypresses
	// https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags

	// we will only do simple key down/up tracking

	// windows wants a '0' returned if we process the event

	switch (uMsg)
	{
		case WM_KEYDOWN:
		{
			keys_down[wParam] = true;
			return 0;
		}
		case WM_KEYUP:
		{
			keys_down[wParam] = false;
			return 0;
		}
		case WM_KILLFOCUS:
		{
			// window/game is about to lose keyboard focus
			// should assume that all keys are now not pressed
			// this prevents a bug where getting focus again the keyup message will never come
			// and the game will think the button is being held until another keyup message is generated by the user
			// pressing the key again
			reset_keys = true;
			return 0;
		}

		// other useful cases left blank
		default:
		case WM_CHAR:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_MOUSELEAVE:
		case WM_MOUSEWHEEL:
			// if we don't process we return a def window proc call
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void RawInput::ProcessFrame(GameInput & processed)
{
	if (reset_keys)
	{
		reset_keys = false;
		for (size_t i = 0; i < 256; ++i)
			keys_down[i] = false;
	}

	processed.lay_bomb = keys_down[VK_SPACE];
	int x_motion = keys_down['A'] - keys_down['D'];
	int y_motion = keys_down['W'] - keys_down['S'];
	processed.motion = glm::vec2(x_motion, y_motion);
}
