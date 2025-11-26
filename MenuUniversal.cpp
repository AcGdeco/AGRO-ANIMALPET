#include "MenuUniversal.h"
#include "Pet.h"
#include "Select.h"
#include "TutoresFuncoes.h"
#include "PetsFuncoes.h"
#include "AgendamentosFuncoes.h"
#include <format>
#include <cwchar> // Adicione esta linha no topo do arquivo, junto com os outros includes

// Função para criar o menu apenas com Arquivo
HMENU CriarMenu() {
    HMENU hMenu = CreateMenu();

    if (!hMenu) return NULL;

    // Menu Home
    HMENU hFileMenuHome = CreatePopupMenu();
    AppendMenuW(hFileMenuHome, MF_STRING, IDM_HOME_INICIO, L"&Inicio\tCtrl+P");
    AppendMenuW(hFileMenuHome, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFileMenuHome, MF_STRING, IDM_HOME_SAIR, L"&Sair\tAlt+F4");

    // Adiciona o menu Home ao menu principal
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenuHome, L"&Home");

    // Menu Tutores
    HMENU hFileMenuTutores = CreatePopupMenu();
    AppendMenuW(hFileMenuTutores, MF_STRING, IDM_TUTORES_NOVO, L"&Criar\tCtrl+Q");
    AppendMenuW(hFileMenuTutores, MF_STRING, IDM_TUTORES_CONSULTAR, L"&Consultar\tCtrl+W");
    AppendMenuW(hFileMenuTutores, MF_STRING, IDM_TUTORES_CONSULTAR, L"&Editar\tCtrl+E");
    AppendMenuW(hFileMenuTutores, MF_STRING, IDM_TUTORES_CONSULTAR, L"&Deletar\tCtrl+R");

    // Adiciona o menu Arquivo ao menu principal
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenuTutores, L"&Tutores");

    // Menu Pets
    HMENU hFileMenuPets = CreatePopupMenu();
    AppendMenuW(hFileMenuPets, MF_STRING, IDM_PETS_NOVO, L"&Criar\tCtrl+A");
    AppendMenuW(hFileMenuPets, MF_STRING, IDM_PETS_CONSULTAR, L"&Consultar\tCtrl+S");
    AppendMenuW(hFileMenuPets, MF_STRING, IDM_PETS_CONSULTAR, L"&Editar\tCtrl+D");
    AppendMenuW(hFileMenuPets, MF_STRING, IDM_PETS_CONSULTAR, L"&Deletar\tCtrl+F");

    // Adiciona o menu Arquivo ao menu principal
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenuPets, L"&Pets");

	// Menu Agendamentos
    HMENU hFileMenuAgendamentos = CreatePopupMenu();
    AppendMenuW(hFileMenuAgendamentos, MF_STRING, IDM_AGENDAMENTOS_NOVO, L"&Criar\tCtrl+Z");
    AppendMenuW(hFileMenuAgendamentos, MF_STRING, IDM_AGENDAMENTOS_CONSULTAR, L"&Consultar\tCtrl+X");
    AppendMenuW(hFileMenuAgendamentos, MF_STRING, IDM_AGENDAMENTOS_CONSULTAR, L"&Editar\tCtrl+C");
    AppendMenuW(hFileMenuAgendamentos, MF_STRING, IDM_AGENDAMENTOS_CONSULTAR, L"&Deletar\tCtrl+V");

    // Adiciona o menu Arquivo ao menu principal
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenuAgendamentos, L"&Agendamentos");

    // Menu Arquivo
    //HMENU hFileMenu = CreatePopupMenu();
    //AppendMenuW(hFileMenu, MF_STRING, IDM_ARQUIVO_NOVO, L"&Criar\tCtrl+T");
    //AppendMenuW(hFileMenu, MF_STRING, IDM_ARQUIVO_CONSULTAR, L"&Consultar\tCtrl+Y");
    //AppendMenuW(hFileMenu, MF_STRING, IDM_ARQUIVO_CONSULTAR, L"&Editar\tCtrl+U");
    //AppendMenuW(hFileMenu, MF_STRING, IDM_ARQUIVO_CONSULTAR, L"&Deletar\tCtrl+I");
   
    // Adiciona o menu Arquivo ao menu principal
    //AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"&Tudo");

    // Menu Ajuda
    HMENU hFileMenuAjuda = CreatePopupMenu();
    AppendMenuW(hFileMenuAjuda, MF_STRING, IDM_AJUDA_SOBRE, L"&Sobre\tCtrl+G");
    
    // Adiciona o menu Ajuda ao menu principal
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenuAjuda, L"&Ajuda");

    AppendMenuW(hMenu, MF_POPUP, IDM_VOLTAR, L"&Voltar");
  
    return hMenu;
}

// Função para processar comandos do menu Arquivo
LRESULT ProcessarMenuArquivo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_COMMAND) {
        int wmId = LOWORD(wParam);

        switch (wmId) {
            case IDM_ARQUIVO_NOVO:
            {
                if (!CreateNewWindow(hWnd, hInst, L"JanelaAddClasse", L"CADASTRO DO AGENDAMENTO"))
                {
                    // O erro já é tratado dentro da função
                    break;
                }
                break;
                return 1;
            }
            case IDM_ARQUIVO_CONSULTAR:
            {
                if (!CreateNewWindow(hWnd, hInst, L"JanelaSelectClasse", L"AGENDAMENTOS"))
                {
				    RecarregarDadosTabela(hWnd);
                    // O erro já é tratado dentro da função
                    break;
                }
                break;
                return 1;
            }
            case IDM_TUTORES_NOVO:
            {
                if (!CreateNewWindow(hWnd, hInst, L"JanelaTutoresAddClasse", L"CADASTRO DO CLIENTE"))
                {
                    // O erro já é tratado dentro da função
                    break;
                }
                break;
                return 1;
            }
            case IDM_TUTORES_CONSULTAR:
            {
                if (!CreateNewWindow(hWnd, hInst, L"JanelaTutoresSelectClasse", L"TUTORES"))
                {
                    TutoresSelect_RecarregarDadosTabela(hWnd);
                    // O erro já é tratado dentro da função
                    break;
                }
                break;
                return 1;
            }
            case IDM_PETS_NOVO:
            {
                if (!CreateNewWindow(hWnd, hInst, L"JanelaPetsAddClasse", L"CADASTRO DO PET"))
                {
                    // O erro já é tratado dentro da função
                    break;
                }
                break;
                return 1;
            }
            case IDM_PETS_CONSULTAR:
            {
                if (!CreateNewWindow(hWnd, hInst, L"JanelaPetsSelectClasse", L"PETS"))
                {
                    PetsSelect_RecarregarDadosTabela(hWnd);
                    // O erro já é tratado dentro da função
                    break;
                }
                break;
                return 1;
            }
            case IDM_AGENDAMENTOS_NOVO:
            {
                if (!CreateNewWindow(hWnd, hInst, L"JanelaAgendamentosAddClasse", L"CADASTRO DO AGENDAMENTO"))
                {
                    // O erro já é tratado dentro da função
                    break;
                }
                break;
                return 1;
            }
            case IDM_AGENDAMENTOS_CONSULTAR:
            {
                if (!CreateNewWindow(hWnd, hInst, L"JanelaAgendamentosSelectClasse", L"AGENDAMENTOS"))
                {
                    AgendamentosSelect_RecarregarDadosTabela(hWnd);
                    // O erro já é tratado dentro da função
                    break;
                }
                break;
                return 1;
            }
            case IDM_VOLTAR:
            {
                if (g_historicoJanelas.size() >= 2) {
                    JanelaAtivaInfo anterior = g_historicoJanelas[g_historicoJanelas.size() - 2];

                    // Recriar a janela com base na classe
                    if (!CreateNewWindow(hWnd, hInst, anterior.className.c_str(), anterior.windowTitle.c_str())) {
                        MessageBoxW(hWnd, L"Erro ao reabrir a janela anterior.", L"Voltar", MB_OK | MB_ICONERROR);
                    }
                }
                else {
                    //MessageBoxW(hWnd, L"Nenhuma janela anterior registrada.", L"Voltar", MB_OK | MB_ICONWARNING);
                }
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
