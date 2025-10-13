#include "MenuUniversal.h"
#include "Pet.h"
#include "Select.h"
#include <format>
#include <cwchar> // Adicione esta linha no topo do arquivo, junto com os outros includes

// Função para criar o menu apenas com Arquivo
HMENU CriarMenu() {
    HMENU hMenu = CreateMenu();

    if (!hMenu) return NULL;

    // Menu Home
    HMENU hFileMenuHome = CreatePopupMenu();
    AppendMenuW(hFileMenuHome, MF_STRING, IDM_HOME_INICIO, L"&Inicio\tCtrl+I");
    AppendMenuW(hFileMenuHome, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFileMenuHome, MF_STRING, IDM_HOME_SAIR, L"&Sair\tAlt+F4");

    // Adiciona o menu Home ao menu principal
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenuHome, L"&Home");

    // Menu Arquivo
    HMENU hFileMenu = CreatePopupMenu();
    AppendMenuW(hFileMenu, MF_STRING, IDM_ARQUIVO_NOVO, L"&Criar\tCtrl+N");
    AppendMenuW(hFileMenu, MF_STRING, IDM_ARQUIVO_CONSULTAR, L"&Consultar\tCtrl+O");
    AppendMenuW(hFileMenu, MF_STRING, IDM_ARQUIVO_CONSULTAR, L"&Editar\tCtrl+E");
    AppendMenuW(hFileMenu, MF_STRING, IDM_ARQUIVO_CONSULTAR, L"&Deletar\tCtrl+D");
   
    // Adiciona o menu Arquivo ao menu principal
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"&Registros");

    // Menu Ajuda
    HMENU hFileMenuAjuda = CreatePopupMenu();
    AppendMenuW(hFileMenuAjuda, MF_STRING, IDM_AJUDA_SOBRE, L"&Sobre\tCtrl+A");
    
    // Adiciona o menu Ajuda ao menu principal
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenuAjuda, L"&Ajuda");
    
    return hMenu;
}

// Função para processar comandos do menu Arquivo
LRESULT ProcessarMenuArquivo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_COMMAND) {
        int wmId = LOWORD(wParam);

        switch (wmId) {
        case IDM_ARQUIVO_NOVO:
        {
            if (!CreateNewWindow(hWnd, hInst, L"JanelaAddClasse", L"AGRO ANIMAL PET - CRIAR REGISTRO"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            break;
            return 1;
        }
        case IDM_ARQUIVO_CONSULTAR:
        {
            if (!CreateNewWindow(hWnd, hInst, L"JanelaSelectClasse", L"AGRO ANIMAL PET - REGISTROS"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            break;
            return 1;
        }
        }
    }
    return 0;
}

LRESULT ProcessarMenuAjuda(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_COMMAND) {
        int wmId = LOWORD(wParam);

        switch (wmId) {
        case IDM_AJUDA_SOBRE:
            MessageBoxW(hWnd, L"Versão 1.0\nDesenvolvido por André Cristian Gonçalves", L"Sobre", MB_OK | MB_ICONINFORMATION);
            return 1;
        }
    }
    return 0;
}

LRESULT ProcessarMenuHome(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_COMMAND) {
        int wmId = LOWORD(wParam);

        switch (wmId) {
        case IDM_HOME_INICIO:
        {
            if (!CreateNewWindow(hWnd, hInst, L"HomeClass", L"AGRO ANIMALPET"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            break;
            return 1;
        }
        case IDM_HOME_SAIR:
        {
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            windowClose(hWnd, message, wParam, lParam);
            PostQuitMessage(0);
            return 1;
        }
        }
    }
    return 0;
}
