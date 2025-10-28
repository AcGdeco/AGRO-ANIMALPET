#pragma once
#include <windows.h>
#include "Global.h"

extern HINSTANCE hInst;

LRESULT CALLBACK WndProcAgendamentosAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitAgendamentosAdd(HINSTANCE hInstance);
BOOL RegisterAgendamentosAddClass(HINSTANCE hInstance); // Nova função