#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Global.h"

extern HINSTANCE hInst;
extern std::vector<std::vector<std::wstring>> PetsSelect_g_tableDataEditar;

LRESULT CALLBACK WndProcPetsEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitPetsEdit(HINSTANCE hInstance);
BOOL RegisterPetsEditClass(HINSTANCE hInstance); // Nova função
void PetsSelect_selectBD();