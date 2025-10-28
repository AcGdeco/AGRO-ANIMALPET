#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Global.h"

extern HINSTANCE hInst;
extern std::vector<std::vector<std::wstring>> TutoresSelect_g_tableData;
extern std::vector<int> TutoresSelect_naoDesenhar;
extern bool TutoresSelect_g_wasInactive; // Indica se a janela estava inativa ou minimizada
extern HWND TutoresSelect_g_hWndMain;    // Handle da janela principal

std::wstring TutoresSelect_utf8_to_wstring(const char* str);
BOOL RegisterTutoresSelectClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProcTutoresSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
