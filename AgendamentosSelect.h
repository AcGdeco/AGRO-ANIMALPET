#pragma once
#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Global.h"

extern HINSTANCE hInst;
extern std::vector<std::vector<std::wstring>> AgendamentosSelect_g_tableData;
extern std::vector<int> AgendamentosSelect_naoDesenhar;
extern bool AgendamentosSelect_g_wasInactive; // Indica se a janela estava inativa ou minimizada
extern HWND AgendamentosSelect_g_hWndMain;    // Handle da janela principal

std::wstring AgendamentosSelect_utf8_to_wstring(const char* str);
BOOL RegisterAgendamentosSelectClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProcAgendamentosSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
