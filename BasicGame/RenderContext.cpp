#include "RenderContext.h"
#include <GL/glew.h>
#include <GL\wglew.h>
#include "WindowsWrapper.h"

RenderContext::RenderContext()
{
	this->glrc = nullptr;
	this->hdc = nullptr;
	this->hinstance = nullptr;
	this->hwnd = nullptr;
}

bool RenderContext::CreateApplicationWindow(glm::ivec2 position, glm::ivec2 resolution)
{
	const wchar_t * className = L"BasicGame C++ YEG demo RDmytruk";
	const wchar_t * titleName = L"Basic Game Demo";

	WNDCLASS wc;				// Window class structure
	DWORD dwExStyle;			// Window extended style
	DWORD dwStyle;				// Window style

	// Windows screen coordinates are left->right, top->bottom
	// OpenGL screen coordinates are left->right, bottom->top
	RECT windowRect;
	windowRect.left = static_cast<long>(position.x);
	windowRect.right = static_cast<long>(position.x + resolution.x);
	windowRect.top = static_cast<long>(position.y);
	windowRect.bottom = static_cast<long>(position.y + resolution.y);

	hinstance = GetModuleHandle(NULL);				// Grab an instance for the window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw on move, and own direct context for the window
	wc.lpfnWndProc = WindowsWrapper::WndProc;		// WndProc handles messages
	wc.cbClsExtra = 0;								// No extra window data
	wc.cbWndExtra = 0;								// No extra window data
	wc.hInstance = hinstance;						// Set the instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load the default icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);		// Load the default arrow pointer
	wc.hbrBackground = NULL;						// No background required (we paint ourselves)
	wc.lpszMenuName = NULL;							// No menu
	wc.lpszClassName = className;					// Set the class name

	// Register the class
	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("Failed to register the window class."), TEXT("ERROR"), MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	dwExStyle = WS_EX_APPWINDOW;		// Window extended style
	//dwStyle = WS_OVERLAPPEDWINDOW;		// Windows style
	dwStyle = WS_OVERLAPPED |
		WS_CAPTION |
		WS_SYSMENU |
		WS_THICKFRAME |			// disabled resizing borders
		WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX;

	// calculates how large of a window we should request based off of our target resolution
	AdjustWindowRectEx(
		&windowRect,
		dwStyle,
		FALSE,		// No windows menu
		dwExStyle);	// Adjust window to requested size

	if (!(hwnd = CreateWindowEx(
		dwExStyle,										// Extended style for the window
		className,										// Class name
		titleName,										// Window title
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,	// Required, required, selected windows style
		windowRect.left,								// Window x-pos
		windowRect.top,									// Window y-pos
		windowRect.right - windowRect.left,				// Adjusted window width
		windowRect.bottom - windowRect.top,				// Adjusted window height
		NULL,											// No parent window
		NULL,											// No menu
		hinstance,										// Instance
		NULL)))											// Don't pass anything to WM_CREATE
	{
		KillGLWindow();	// Reset the display
		MessageBox(NULL, TEXT("Window creation error."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	return true;
}

bool RenderContext::CreateGLWindow()
{
	BYTE bits_per_pixel = 32;
	BYTE depth_buffer_bits = 24;
	BYTE stencil_buffer_bits = 0;

	static PIXELFORMATDESCRIPTOR pfd =	// pfd tells windows how we want things to be
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this pixel format descriptor
		1,								// Version number
		PFD_DRAW_TO_WINDOW |			// Format must support window
		PFD_SUPPORT_OPENGL |			// Format must support OpenGL
		PFD_DOUBLEBUFFER,				// Must support double buffering
		PFD_TYPE_RGBA,					// Request an RGBA format
		bits_per_pixel,					// Select our color depth
		0, 0, 0, 0, 0, 0,				// Color bits ignored
		8,								// 8-bit alpha buffer
		0,								// Shift bit ignored
		0,								// No accumulation buffer
		0, 0, 0, 0,						// Accumulation bits ignored
		depth_buffer_bits,				// depth buffer bits
		stencil_buffer_bits,			// stencil buffer bits
		0,								// No auxiliary buffer
		PFD_MAIN_PLANE,					// Main drawing layer
		0,								// Reserved
		0, 0, 0							// Layer masks ignored
	};

	// Get device context
	if (!(hdc = GetDC(hwnd)))
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("Can't create a GL device context."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// Windows chooses pixel format
	int pixel_format;
	if (!(pixel_format = ChoosePixelFormat(hdc, &pfd)))
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("Can't find a suitable PixelFormat."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// Set pixel format
	if (!SetPixelFormat(hdc, pixel_format, &pfd))
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("Can't set the PixelFormat."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// render context
	if (!(glrc = wglCreateContext(hdc)))
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("Can't create a GL rendering context."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!wglMakeCurrent(hdc, glrc))
	{
		KillGLWindow();
		MessageBox(NULL, TEXT("Can't activate the GL rendering context."), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// initialize glew
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();

	return true;
}

void RenderContext::ShowApplicationWindow()
{
	ShowWindow(hwnd, SW_SHOW);	// show the window
	SetForegroundWindow(hwnd);	// slightly higher priority
	SetFocus(hwnd);				// sets input focus to the window
}

void RenderContext::KillGLWindow()
{
	if (glrc)
	{
		// there is a render context
		try
		{
			if (!wglMakeCurrent(NULL, NULL))	// Are we able to release the device and rendering contexts?
			{
				MessageBox(NULL, TEXT("Release of Device Context and Rendering Context Failed."), TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
			}

			if (!wglDeleteContext(glrc)) // Are we able to delete the rendering context?
			{
				MessageBox(NULL, TEXT("Release Rendering Context Failed."), TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
			}
		}
		catch (...)
		{
			// catch errors but for now don't care about reporting
		}

		glrc = nullptr;
	}
}

void RenderContext::SwapBuffer()
{
	SwapBuffers(hdc);
}

void RenderContext::SetVSync(bool enabled)
{
	// requires wgl
	if (enabled)
		wglSwapIntervalEXT(1);
	else
		wglSwapIntervalEXT(0);
}
