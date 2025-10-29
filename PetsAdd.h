#pragma once
#include <windows.h>
#include "Global.h"

extern HINSTANCE hInst;
extern HWND PetsSelect_g_hButton_consultar;

LRESULT CALLBACK WndProcPetsAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitPetsAdd(HINSTANCE hInstance);
BOOL RegisterPetsAddClass(HINSTANCE hInstance); // Nova função