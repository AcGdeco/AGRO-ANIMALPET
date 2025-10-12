#pragma once

#include <windows.h>

// IDs dos itens do menu
#define IDM_HOME_INICIO 1000
#define IDM_ARQUIVO_NOVO     1001
#define IDM_ARQUIVO_CONSULTAR    1002
#define IDM_ARQUIVO_SAIR     1003
#define IDM_AJUDA_SOBRE 1004
#define IDM_HOME_SAIR 1005

// Funções do Menu Simples
HMENU CriarMenu();
LRESULT ProcessarMenuArquivo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT ProcessarMenuAjuda(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT ProcessarMenuHome(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);