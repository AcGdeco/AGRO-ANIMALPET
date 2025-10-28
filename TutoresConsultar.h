#pragma once
#include <windows.h>
#include "Global.h"

extern HINSTANCE hInst;

LRESULT CALLBACK WndProcTutoresRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitTutoresRead(HINSTANCE hInstance);
BOOL RegisterTutoresReadClass(HINSTANCE hInstance); // Nova função