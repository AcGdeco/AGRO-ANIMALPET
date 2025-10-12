#pragma once
#include <windows.h>

extern HINSTANCE hInst;

LRESULT CALLBACK NewWndProcRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitRead(HINSTANCE hInstance);
BOOL RegisterReadClass(HINSTANCE hInstance); // Nova função