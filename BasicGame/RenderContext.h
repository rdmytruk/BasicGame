#pragma once
#include <Windows.h>
#include <glm/vec2.hpp>

class RenderContext
{
	// windows specific variables we keep private to not contaminate other code
private:
	HDC hdc;
	HINSTANCE hinstance;
	HWND hwnd;
	// Permanent rendering context
	HGLRC glrc;
public:
	RenderContext();

	bool CreateApplicationWindow(glm::ivec2 position, glm::ivec2 resolution);
	bool CreateGLWindow();
	void ShowApplicationWindow();
	void KillGLWindow();
	void SwapBuffer();
	void SetVSync(bool enabled);
};

