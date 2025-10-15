#pragma once
#include <windows.h>
#include <vector>
#include <string>

extern HINSTANCE hInst;
extern std::vector<std::vector<std::wstring>> g_tableData;
std::wstring utf8_to_wstring(const char* str);

LRESULT CALLBACK NewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitSelect(HINSTANCE hInstance);
BOOL RegisterSelectClass(HINSTANCE hInstance); // Nova função
