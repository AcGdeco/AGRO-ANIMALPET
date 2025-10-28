#pragma once
#include <windows.h>
#include "Global.h"

extern HINSTANCE hInst;

LRESULT CALLBACK WndProcTutoresAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitTutoresAdd(HINSTANCE hInstance);
BOOL RegisterTutoresAddClass(HINSTANCE hInstance); // Nova função