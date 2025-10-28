#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Global.h"

extern HINSTANCE hInst;
extern std::vector<std::vector<std::wstring>> TutoresSelect_g_tableDataEditar;

LRESULT CALLBACK WndProcTutoresEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitTutoresEdit(HINSTANCE hInstance);
BOOL RegisterTutoresEditClass(HINSTANCE hInstance); // Nova função
void TutoresSelect_selectBD();