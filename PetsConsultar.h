#pragma once
#include <windows.h>
#include "Global.h"

extern HINSTANCE hInst;

LRESULT CALLBACK WndProcPetsRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitPetsRead(HINSTANCE hInstance);
BOOL RegisterPetsReadClass(HINSTANCE hInstance); // Nova função