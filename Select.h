#pragma once
#include <windows.h>
#include <vector>
#include <string>

extern HINSTANCE hInst;
extern std::vector<std::vector<std::wstring>> g_tableData;
extern std::vector<int> naoDesenhar;
extern bool g_wasInactive; // Indica se a janela estava inativa ou minimizada
extern HWND g_hWndMain;    // Handle da janela principal

std::wstring utf8_to_wstring(const char* str);

LRESULT CALLBACK NewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitSelect(HINSTANCE hInstance);
BOOL RegisterSelectClass(HINSTANCE hInstance); // Nova função
