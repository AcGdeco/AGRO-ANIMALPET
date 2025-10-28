#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Global.h"

extern HINSTANCE hInst;
extern std::vector<std::vector<std::wstring>> AgendamentosSelect_g_tableDataEditar;

LRESULT CALLBACK WndProcAgendamentosEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitAgendamentosEdit(HINSTANCE hInstance);
BOOL RegisterAgendamentosEditClass(HINSTANCE hInstance); // Nova função
void AgendamentosSelect_selectBD();