#pragma once
#include <windows.h>

extern HINSTANCE hInst;

LRESULT CALLBACK NewWndProcAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitAdd(HINSTANCE hInstance);
BOOL RegisterAddClass(HINSTANCE hInstance); // Nova função