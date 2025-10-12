#pragma once

#include "resource.h"

extern int windowsNumber;

extern int g_scrollX;
extern int g_scrollY;
extern int g_contentWidth;
extern int g_contentHeight;
extern int g_clientWidth;
extern int g_clientHeight;

BOOL CreateNewWindow(HWND hWndParent, HINSTANCE hInst, LPCWSTR className, LPCWSTR windowTittle);
BOOL ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Shortcuts(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL windowClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL scroll(HWND hWnd, int scrollX, int scrollY, int contentWidth, int contentHeight, int clientWidth, int clientHeight);