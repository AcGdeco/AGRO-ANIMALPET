#pragma once

#include "resource.h"

extern int windowsNumber;

BOOL CreateNewWindow(HWND hWndParent, HINSTANCE hInst, LPCWSTR className, LPCWSTR windowTittle);
BOOL ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Shortcuts(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL windowClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);