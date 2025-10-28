#pragma once
#include <windows.h>
#include "Global.h"

extern HINSTANCE hInst;

LRESULT CALLBACK WndProcAgendamentosRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitAgendamentosRead(HINSTANCE hInstance);
BOOL RegisterAgendamentosReadClass(HINSTANCE hInstance); // Nova função