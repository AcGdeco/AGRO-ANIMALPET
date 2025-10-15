#pragma once
#include <windows.h>

extern HINSTANCE hInst;

LRESULT CALLBACK NewWndProcEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitEdit(HINSTANCE hInstance);
BOOL RegisterEditClass(HINSTANCE hInstance); // Nova função