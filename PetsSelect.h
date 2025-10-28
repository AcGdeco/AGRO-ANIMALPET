#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Global.h"

extern HINSTANCE hInst;
extern std::vector<std::vector<std::wstring>> PetsSelect_g_tableData;
extern std::vector<int> PetsSelect_naoDesenhar;
extern bool PetsSelect_g_wasInactive; // Indica se a janela estava inativa ou minimizada
extern HWND PetsSelect_g_hWndMain;    // Handle da janela principal

std::wstring PetsSelect_utf8_to_wstring(const char* str);
BOOL RegisterPetsSelectClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProcPetsSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
