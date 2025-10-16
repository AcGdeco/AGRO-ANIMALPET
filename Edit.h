#pragma once
#include <windows.h>
#include <vector>
#include <string>

extern HINSTANCE hInst;
extern std::vector<std::vector<std::wstring>> g_tableDataEditar;

LRESULT CALLBACK NewWndProcEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitEdit(HINSTANCE hInstance);
BOOL RegisterEditClass(HINSTANCE hInstance); // Nova função