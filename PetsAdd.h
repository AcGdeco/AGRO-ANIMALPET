#pragma once
#include <windows.h>
#include "Global.h"

extern HINSTANCE hInst;

LRESULT CALLBACK WndProcPetsAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitPetsAdd(HINSTANCE hInstance);
BOOL RegisterPetsAddClass(HINSTANCE hInstance); // Nova função