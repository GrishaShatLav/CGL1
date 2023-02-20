#pragma once
#include "Game.h"

class Game;

class DisplayWin32 {
protected:
	LPCWSTR applicationName;
	RECT windowRect;
	HINSTANCE hInstance;
	HWND hWnd;
	WNDCLASSEX wc;
	int clientHeight;
	int clientWidth;

public:
	DisplayWin32(LPCWSTR& applicationName, int clientWidth, int clientHeight, WNDPROC wndProc);

	int GetClientWidth();
	int GetClientHeight();
	HWND& GetHWnd();
};
