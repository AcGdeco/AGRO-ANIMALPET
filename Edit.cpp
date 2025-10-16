#include "Edit.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "Pet.h"
#include <string>
#include "sqlite3.h"
#include <vector>
#include <uxtheme.h>
#include <regex>
#include <cctype>
#pragma comment(lib, "uxtheme.lib")

// IDs dos controles
#define ID_RADIO_EDIT_PADRAO 2015
#define ID_RADIO_EDIT_HIDRATACAO 2016
#define ID_RADIO_EDIT_BANHO_NENHUM 2017

#define ID_RADIO_EDIT_TESOURA 2018
#define ID_RADIO_EDIT_MAQUINA 2019
#define ID_RADIO_EDIT_HIGIENICA 2020
#define ID_RADIO_EDIT_TOSADARACA 2021
#define ID_RADIO_EDIT_TOSA_NENHUM 2022

#define ID_CHECKBOX_EDIT_PULGAS 2023
#define ID_CHECKBOX_EDIT_CARRAPATOS 2024

#define ID_CHECKBOX_EDIT_PELE 2025
#define ID_CHECKBOX_EDIT_OLHOS 2026
#define ID_CHECKBOX_EDIT_SECRECAO 2027
#define ID_CHECKBOX_EDIT_OUVIDO 2028

std::vector<std::vector<std::wstring>> g_tableDataEditar;

void selectBD() {
    // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
    g_tableDataEditar.clear();

    // Consultar o banco apenas se a tabela estiver vazia
    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("pet.db", &db);
    if (rc == SQLITE_OK) {
        std::string idRecordStr = std::to_string(idRecord);
        std::string sqlSelect = "SELECT * FROM Pets WHERE ID = '" + idRecordStr + "';";

        rc = sqlite3_exec(db, sqlSelect.c_str(), sqlite_callback, &g_tableDataEditar, &errMsg);
        if (rc != SQLITE_OK) {
            if (errMsg) {
                // Converte char* para wchar_t* corretamente
                size_t len = strlen(errMsg) + 1;
                std::wstring wErrMsg(len, L'\0');
                size_t convertedChars = 0;
                mbstowcs_s(&convertedChars, &wErrMsg[0], len, errMsg, _TRUNCATE);
                // Remove o caractere nulo extra do final
                if (convertedChars > 0) {
                    wErrMsg.resize(convertedChars - 1);
                }
                g_tableDataEditar.push_back({ L"Erro", wErrMsg });
                sqlite3_free(errMsg);
            }
            else {
                g_tableDataEditar.push_back({ L"Erro", L"Erro desconhecido no SQLite: " + std::to_wstring(rc) });
            }
        }
        else {
            // Sucesso - talvez adicionar uma mensagem de confirmação
            if (g_tableDataEditar.empty()) {
                g_tableDataEditar.push_back({ L"Info", L"Nenhum registro encontrado com ID: " + std::to_wstring(idRecord) });
            }
        }

        sqlite3_close(db);
    }
    else {
        g_tableDataEditar.push_back({ L"Erro", L"Não foi possível abrir o banco" });
    }
}

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para registrar a classe da janela
BOOL RegisterEditClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcEdit;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"JanelaEditClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Edit COM SCROLL CORRIGIDO
LRESULT CALLBACK WndProcEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Processar o menu APENAS para mensagens específicas
    if (message == WM_COMMAND || message == WM_INITMENU || message == WM_MENUSELECT) {
        if (ProcessarMenu(hWnd, message, wParam, lParam)) {
            return 0; // Mensagem já processada pelo menu
        }
    }

    // Depois processa as mensagens específicas da janela
    switch (message)
    {
    case WM_CREATE:
    {
        selectBD();

        // Resetar scroll para garantir que comece do topo
        g_scrollY = 0;
        g_scrollX = 0;

        // Limpar array de controles
        g_editControls.clear();

        ConfigurarScrollBarsAgendamento(hWnd);

        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;

        // Configurar a tabela com scroll
        int cellHeight = 32;
        int numColumns = 21;
        int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
        int startY = 40 - g_scrollY;
        int startX = 22 - g_scrollX;

        // Criar campos de entrada
        for (int col = 0; col < 21; col++) {
            int colNumber = col + 1;
            int controlID = col + 2; // IDs de 2 a 22
            int xPos = startX + cellWidth + 10;
            int yPos = startY + colNumber * cellHeight + 3;

            if (col == 8) {
                HWND hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    NULL,    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(controlID),              // ID único
                    NULL,                       // Instância
                    NULL
                );

                checarInput(hCheckbox, col, L"Sim");

                g_editControls.push_back(hCheckbox);
            }
            else if (col == 11) {
                HWND hRadio;

                // Criar radio button 1
                hRadio = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Padrão",    // Texto do radio button
                    WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    100, 20,                         // Largura ajustada para texto
                    hWnd,                            // Janela pai
                    (HMENU)(ID_RADIO_EDIT_PADRAO),       // ID único
                    NULL,                            // Instância
                    NULL
                );

                checarInput(hRadio, col, L"Padrão");

                SetWindowTheme(hRadio, L"", L""); // Desativar tema para fundo transparente
                g_editControls.push_back(hRadio);

                // Criar radio button 2
                hRadio = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Hidratação",    // Texto do radio button
                    WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Estilos IMPORTANTES
                    xPos + cellWidth + 10, yPos,     // Posição X, Y
                    100, 20,                         // Largura ajustada para texto
                    hWnd,                            // Janela pai
                    (HMENU)(ID_RADIO_EDIT_HIDRATACAO),       // ID único
                    NULL,                            // Instância
                    NULL
                );

                checarInput(hRadio, col, L"Hidratação");

                SetWindowTheme(hRadio, L"", L""); // Desativar tema para fundo transparente
                g_editControls.push_back(hRadio);

                // Criar radio button 3
                hRadio = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Nenhum",    // Texto do radio button
                    WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Estilos IMPORTANTES
                    xPos + 2 * cellWidth + 10, yPos,     // Posição X, Y
                    100, 20,                         // Largura ajustada para texto
                    hWnd,                            // Janela pai
                    (HMENU)(ID_RADIO_EDIT_BANHO_NENHUM),       // ID único
                    NULL,                            // Instância
                    NULL
                );

                checarInput(hRadio, col, L"Nenhum");

                SetWindowTheme(hRadio, L"", L""); // Desativar tema para fundo transparente
                g_editControls.push_back(hRadio);
            }
            else if (col == 12) {
                HWND hRadio;

                // Criar radio button 1
                hRadio = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Tesoura",    // Texto do radio button
                    WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    100, 20,                         // Largura ajustada para texto
                    hWnd,                            // Janela pai
                    (HMENU)(ID_RADIO_EDIT_TESOURA),       // ID único
                    NULL,                            // Instância
                    NULL
                );

                checarInput(hRadio, col, L"Tesoura");

                SetWindowTheme(hRadio, L"", L""); // Desativar tema para fundo transparente
                g_editControls.push_back(hRadio);

                // Criar radio button 2
                hRadio = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Máquina",    // Texto do radio button
                    WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Estilos IMPORTANTES
                    xPos + cellWidth + 10, yPos,     // Posição X, Y
                    100, 20,                         // Largura ajustada para texto
                    hWnd,                            // Janela pai
                    (HMENU)(ID_RADIO_EDIT_MAQUINA),       // ID único
                    NULL,                            // Instância
                    NULL
                );

                checarInput(hRadio, col, L"Máquina");

                SetWindowTheme(hRadio, L"", L""); // Desativar tema para fundo transparente
                g_editControls.push_back(hRadio);

                // Criar radio button 3
                hRadio = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Higiênica",    // Texto do radio button
                    WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Estilos IMPORTANTES
                    xPos + 2 * cellWidth + 10, yPos,     // Posição X, Y
                    100, 20,                         // Largura ajustada para texto
                    hWnd,                            // Janela pai
                    (HMENU)(ID_RADIO_EDIT_HIGIENICA),       // ID único
                    NULL,                            // Instância
                    NULL
                );

                checarInput(hRadio, col, L"Higiênica");

                SetWindowTheme(hRadio, L"", L""); // Desativar tema para fundo transparente
                g_editControls.push_back(hRadio);

                // Criar radio button 4
                hRadio = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Tosa da Raça",    // Texto do radio button
                    WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Estilos IMPORTANTES
                    xPos + 3 * cellWidth + 10, yPos,     // Posição X, Y
                    100, 20,                         // Largura ajustada para texto
                    hWnd,                            // Janela pai
                    (HMENU)(ID_RADIO_EDIT_TOSADARACA),       // ID único
                    NULL,                            // Instância
                    NULL
                );

                checarInput(hRadio, col, L"Tosa da Raça");

                SetWindowTheme(hRadio, L"", L""); // Desativar tema para fundo transparente
                g_editControls.push_back(hRadio);

                // Criar radio button 5
                hRadio = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Nenhum",    // Texto do radio button
                    WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Estilos IMPORTANTES
                    xPos + 4 * cellWidth + 10, yPos,     // Posição X, Y
                    100, 20,                         // Largura ajustada para texto
                    hWnd,                            // Janela pai
                    (HMENU)(ID_RADIO_EDIT_TOSA_NENHUM),       // ID único
                    NULL,                            // Instância
                    NULL
                );

                checarInput(hRadio, col, L"Nenhum");

                SetWindowTheme(hRadio, L"", L""); // Desativar tema para fundo transparente
                g_editControls.push_back(hRadio);
            }
            else if (col == 14) {
                HWND hCheckbox;
                hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Pulgas",    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(ID_CHECKBOX_EDIT_PULGAS),              // ID único
                    NULL,                       // Instância
                    NULL
                );

                checarInput(hCheckbox, col, L"Pulgas");

                g_editControls.push_back(hCheckbox);

                hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Carrapatos",    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(ID_CHECKBOX_EDIT_CARRAPATOS),              // ID único
                    NULL,                       // Instância
                    NULL
                );

                checarInput(hCheckbox, col, L"Carrapatos");

                g_editControls.push_back(hCheckbox);
            }
            else if (col == 15) {
                HWND hCheckbox;
                hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Pele",    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(ID_CHECKBOX_EDIT_PELE),              // ID único
                    NULL,                       // Instância
                    NULL
                );

                checarInput(hCheckbox, col, L"Pele");

                g_editControls.push_back(hCheckbox);

                hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Olhos",    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(ID_CHECKBOX_EDIT_OLHOS),              // ID único
                    NULL,                       // Instância
                    NULL
                );

                checarInput(hCheckbox, col, L"Olhos");

                g_editControls.push_back(hCheckbox);

                hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Secreção",    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(ID_CHECKBOX_EDIT_SECRECAO),              // ID único
                    NULL,                       // Instância
                    NULL
                );

                checarInput(hCheckbox, col, L"Secreção");

                g_editControls.push_back(hCheckbox);

                hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Ouvido",    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(ID_CHECKBOX_EDIT_OUVIDO),              // ID único
                    NULL,                       // Instância
                    NULL
                );

                checarInput(hCheckbox, col, L"Ouvido");

                g_editControls.push_back(hCheckbox);
            }
            else {
               
                std::wstring displayText = g_tableDataEditar[1][col + 1];

                HWND hEdit = CreateWindowEx(
                    0, L"EDIT", displayText.c_str(),
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                    xPos, yPos, 200, 25, hWnd, (HMENU)(controlID), NULL, NULL
                );
                g_editControls.push_back(hEdit);
            }
        }

        // Criar botão
        int buttonY = startY + 22 * cellHeight + 3;
        g_hButton = CreateWindowW(
            L"BUTTON", L"Salvar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            startX, buttonY, 150, 30,
            hWnd, (HMENU)(1),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        break;
    }

    case WM_VSCROLL: {
        SCROLLINFO si = {};
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &si);

        int oldPos = si.nPos;

        switch (LOWORD(wParam)) {
        case SB_LINEUP:        si.nPos -= 10; break;
        case SB_LINEDOWN:      si.nPos += 10; break;
        case SB_PAGEUP:        si.nPos -= si.nPage; break;
        case SB_PAGEDOWN:      si.nPos += si.nPage; break;
        case SB_THUMBTRACK:    si.nPos = si.nTrackPos; break;
        case SB_THUMBPOSITION: si.nPos = si.nTrackPos; break;
        case SB_TOP:           si.nPos = si.nMin; break;
        case SB_BOTTOM:        si.nPos = si.nMax; break;
        default: break;
        }

        si.fMask = SIF_POS;
        si.nPos = max(si.nMin, min(si.nPos, si.nMax - (int)si.nPage + 1));
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
        GetScrollInfo(hWnd, SB_VERT, &si);

        if (si.nPos != oldPos) {
            g_scrollY = si.nPos;
            // Atualizar posições dos controles ANTES do redraw
            AtualizarPosicoesControlesAgendamento(hWnd);
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        break;
    }

    case WM_HSCROLL: {
        SCROLLINFO si = {};
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_HORZ, &si);

        int oldPos = si.nPos;

        switch (LOWORD(wParam)) {
        case SB_LINELEFT:      si.nPos -= 10; break;
        case SB_LINERIGHT:     si.nPos += 10; break;
        case SB_PAGELEFT:      si.nPos -= si.nPage; break;
        case SB_PAGERIGHT:     si.nPos += si.nPage; break;
        case SB_THUMBTRACK:    si.nPos = si.nTrackPos; break;
        case SB_THUMBPOSITION: si.nPos = si.nTrackPos; break;
        default: break;
        }

        si.fMask = SIF_POS;
        si.nPos = max(si.nMin, min(si.nPos, si.nMax - (int)si.nPage + 1));
        SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
        GetScrollInfo(hWnd, SB_HORZ, &si);

        if (si.nPos != oldPos) {
            g_scrollX = si.nPos;
            // Atualizar posições dos controles ANTES do redraw
            AtualizarPosicoesControlesAgendamento(hWnd);
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        break;
    }

    case WM_SIZE: {
        int newWidth = LOWORD(lParam);
        int newHeight = HIWORD(lParam);

        g_clientWidth = newWidth;
        g_clientHeight = newHeight;

        ConfigurarScrollBarsAgendamento(hWnd);
        // Atualizar posições dos controles após redimensionamento
        AtualizarPosicoesControlesAgendamento(hWnd);

        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }

    case WM_MOUSEWHEEL: {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

        SCROLLINFO si = {};
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &si);

        int oldPos = si.nPos;
        int scrollAmount = -zDelta / WHEEL_DELTA * 30;

        si.nPos += scrollAmount;
        si.nPos = max(si.nMin, min(si.nPos, si.nMax - (int)si.nPage + 1));

        si.fMask = SIF_POS;
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
        GetScrollInfo(hWnd, SB_VERT, &si);

        if (si.nPos != oldPos) {
            g_scrollY = si.nPos;
            // Atualizar posições dos controles ANTES do redraw
            AtualizarPosicoesControlesAgendamento(hWnd);
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        return 0;
    }

    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wParam;
        HBRUSH hbr = CreateSolidBrush(RGB(30, 30, 30));
        RECT rect;
        GetClientRect(hWnd, &rect);
        FillRect(hdc, &rect, hbr);
        DeleteObject(hbr);
        return 1; // Evitar flicker
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        if (wmId == 1) // Botão "Salvar Registro"
        {
            std::wstring dados[23];
            for (int i = 2; i <= 22; i++) {
                std::wstring controlIDStr = std::to_wstring(i);
                HWND input = GetDlgItem(hWnd, i);

                if (i == 10) {
                    if (input) {
                        std::wstring resposta;
                        if (SendMessage(input, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                            resposta = L"Sim";
                        }
                        else {
                            resposta = L"Não";
                        }
                        dados[i] = std::wstring(resposta);
                    }
                }
                else if (i == 13) { // Banho (radio buttons)
                    HWND hRadio1 = GetDlgItem(hWnd, ID_RADIO_EDIT_PADRAO);
                    HWND hRadio2 = GetDlgItem(hWnd, ID_RADIO_EDIT_HIDRATACAO);
                    HWND hRadio3 = GetDlgItem(hWnd, ID_RADIO_EDIT_BANHO_NENHUM);
                    if (hRadio1 && SendMessage(hRadio1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Padrão";
                    }
                    else if (hRadio2 && SendMessage(hRadio2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Hidratação";
                    }
                    else if (hRadio3 && SendMessage(hRadio3, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Nenhum";
                    }
                }
                else if (i == 14) { // Tosa (radio buttons)
                    HWND hRadio1 = GetDlgItem(hWnd, ID_RADIO_EDIT_TESOURA);
                    HWND hRadio2 = GetDlgItem(hWnd, ID_RADIO_EDIT_MAQUINA);
                    HWND hRadio3 = GetDlgItem(hWnd, ID_RADIO_EDIT_HIGIENICA);
                    HWND hRadio4 = GetDlgItem(hWnd, ID_RADIO_EDIT_TOSADARACA);
                    HWND hRadio5 = GetDlgItem(hWnd, ID_RADIO_EDIT_TOSA_NENHUM);
                    if (hRadio1 && SendMessage(hRadio1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Tesoura";
                    }
                    else if (hRadio2 && SendMessage(hRadio2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Máquina";
                    }
                    else if (hRadio3 && SendMessage(hRadio3, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Higiênica";
                    }
                    else if (hRadio4 && SendMessage(hRadio4, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Tosa da Raça";
                    }
                    else if (hRadio5 && SendMessage(hRadio5, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Nenhum";
                    }
                }
                else if (i == 16) { // Parasitas (radio buttons)
                    HWND hCheckbox1 = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_PULGAS);
                    HWND hCheckbox2 = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_CARRAPATOS);

                    if (hCheckbox1 && SendMessage(hCheckbox1, BM_GETCHECK, 0, 0) == BST_CHECKED && hCheckbox2 && SendMessage(hCheckbox2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Pulgas e Carrapatos";
                    }
                    else if (hCheckbox1 && SendMessage(hCheckbox1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Pulgas";
                    }
                    else if (hCheckbox2 && SendMessage(hCheckbox2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Carrapatos";
                    }
                }
                else if (i == 17) { // Lesões (checkbox buttons)
                    HWND hCheckboxPele = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_PELE);
                    HWND hCheckboxOlhos = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_OLHOS);
                    HWND hCheckboxSecrecao = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_SECRECAO);
                    HWND hCheckboxOuvido = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_OUVIDO);

                    // Verificar estado de cada checkbox
                    bool peleChecked = hCheckboxPele && SendMessage(hCheckboxPele, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    bool olhosChecked = hCheckboxOlhos && SendMessage(hCheckboxOlhos, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    bool secrecaoChecked = hCheckboxSecrecao && SendMessage(hCheckboxSecrecao, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    bool ouvidoChecked = hCheckboxOuvido && SendMessage(hCheckboxOuvido, BM_GETCHECK, 0, 0) == BST_CHECKED;

                    // Contar quantos estão selecionados
                    int countChecked = (peleChecked ? 1 : 0) + (olhosChecked ? 1 : 0) +
                        (secrecaoChecked ? 1 : 0) + (ouvidoChecked ? 1 : 0);

                    // Lógica para determinar o texto baseado nas combinações
                    if (countChecked == 0) {
                        dados[i] = L"N/A"; // ou L"", dependendo do que você preferir
                    }
                    else if (countChecked == 4) {
                        dados[i] = L"Pele, Olhos, Secreção e Ouvido";
                    }
                    else if (countChecked == 3) {
                        if (!peleChecked) dados[i] = L"Olhos, Secreção e Ouvido";
                        else if (!olhosChecked) dados[i] = L"Pele, Secreção e Ouvido";
                        else if (!secrecaoChecked) dados[i] = L"Pele, Olhos e Ouvido";
                        else dados[i] = L"Pele, Olhos e Secreção";
                    }
                    else if (countChecked == 2) {
                        if (peleChecked && olhosChecked) dados[i] = L"Pele e Olhos";
                        else if (peleChecked && secrecaoChecked) dados[i] = L"Pele e Secreção";
                        else if (peleChecked && ouvidoChecked) dados[i] = L"Pele e Ouvido";
                        else if (olhosChecked && secrecaoChecked) dados[i] = L"Olhos e Secreção";
                        else if (olhosChecked && ouvidoChecked) dados[i] = L"Olhos e Ouvido";
                        else dados[i] = L"Secreção e Ouvido";
                    }
                    else { // countChecked == 1
                        if (peleChecked) dados[i] = L"Pele";
                        else if (olhosChecked) dados[i] = L"Olhos";
                        else if (secrecaoChecked) dados[i] = L"Secreção";
                        else dados[i] = L"Ouvido";
                    }
                }
                else {
                    if (input) {
                        wchar_t buffer[256];
                        GetWindowText(input, buffer, 256);
                        dados[i] = std::wstring(buffer);
                    }
                }
            }

            sqlite3* db;
            char* errMsg = 0;
            int rc = sqlite3_open("pet.db", &db);
            if (rc) {
                MessageBox(hWnd, L"Erro ao abrir/criar o banco de dados!", L"Erro", MB_OK | MB_ICONERROR);
            }
            else {
                std::wstring currentDate = GetCurrentDate();
                std::wstring currentHour = GetCurrentHour();

                std::wstring sqlInsertW = L"UPDATE Pets SET "
                    L"Nome_do_Pet = '" + treatDataAppointment(dados[2], 2) + L"', "
                    L"Raca = '" + treatDataAppointment(dados[3], 3) + L"', "
                    L"Nome_do_Tutor = '" + treatDataAppointment(dados[4], 4) + L"', "
                    L"CEP = '" + treatDataAppointment(dados[5], 5) + L"', "
                    L"Cor = '" + treatDataAppointment(dados[6], 6) + L"', "
                    L"Idade = '" + treatDataAppointment(dados[7], 7) + L"', "
                    L"Peso = '" + treatDataAppointment(dados[8], 8) + L"', "
                    L"Sexo = '" + treatDataAppointment(dados[9], 9) + L"', "
                    L"Castrado = '" + treatDataAppointment(dados[10], 10) + L"', "
                    L"Endereco = '" + treatDataAppointment(dados[11], 11) + L"', "
                    L"Ponto_de_referencia = '" + treatDataAppointment(dados[12], 12) + L"', "
                    L"Banho = '" + treatDataAppointment(dados[13], 13) + L"', "
                    L"Tosa = '" + treatDataAppointment(dados[14], 14) + L"', "
                    L"Obs_Tosa = '" + treatDataAppointment(dados[15], 15) + L"', "
                    L"Parasitas = '" + treatDataAppointment(dados[16], 16) + L"', "
                    L"Lesoes = '" + treatDataAppointment(dados[17], 17) + L"', "
                    L"Obs_Lesoes = '" + treatDataAppointment(dados[18], 18) + L"', "
                    L"Telefone = '" + treatDataAppointment(dados[19], 19) + L"', "
                    L"CPF = '" + treatDataAppointment(dados[20], 20) + L"', "
                    L"Appointment_Date = '" + treatDataAppointment(dados[21], 21) + L"', "
                    L"Appointment_Hour = '" + treatDataAppointment(dados[22], 22) + L"', "
                    L"Date = '" + currentDate + L"', "
                    L"Hour = '" + currentHour + L"' "
                    L"WHERE ID = " + std::to_wstring(idRecord) + L";";

                if (error == L"1") {
                    MessageBox(hWnd, msg, L"Erro", MB_OK | MB_ICONERROR);
                    error = L"0";
                    mensagem.clear();
                }
                else {
                    std::string sqlInsertUtf8 = WideToUTF8(sqlInsertW);
                    errMsg = nullptr;
                    rc = sqlite3_exec(db, sqlInsertUtf8.c_str(), nullptr, nullptr, &errMsg);

                    if (rc != SQLITE_OK && errMsg) {
                        std::wstring wErrMsg = UTF8ToWide(errMsg);
                        MessageBox(hWnd, wErrMsg.c_str(), L"Erro", MB_OK | MB_ICONERROR);
                        sqlite3_free(errMsg);
                    }
                    else {
                        updateWindow(L"JanelaSelectClasse");
                        MessageBox(hWnd, L"Dados inseridos com sucesso!", L"Sucesso", MB_OK);
                    }
                }
                sqlite3_close(db);
            }
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Double buffering
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, g_clientWidth, g_clientHeight);
        HGDIOBJ hOld = SelectObject(hdcMem, hbmMem);

        // Preencher fundo completo com branco
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        FillRect(hdcMem, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));

        fonte(L"Font", RGB(0, 0, 0), hdcMem);

        int width = g_clientWidth - 44;
        int cellHeight = 32;
        int startY = 40 - g_scrollY;
        int startX = 22 - g_scrollX;

        // Título com fundo branco
        RECT titleRect = { startX - 5, startY - 25, startX + 250, startY + 5 };
        FillRect(hdcMem, &titleRect, (HBRUSH)(COLOR_WINDOW + 1));
        windowsTitle(hdcMem, startX, startY - 20, L"EDITAR AGENDAMENTO", 18);

        // Desenhar linhas visíveis
        int firstVisibleRow = max(0, (g_scrollY - 40) / cellHeight);
        int lastVisibleRow = min(20, firstVisibleRow + (g_clientHeight / cellHeight) + 2);

        HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));

        // CORREÇÃO: Usar background OPAQUE com cores consistentes
        SetBkMode(hdcMem, OPAQUE);

        for (int row = firstVisibleRow; row <= lastVisibleRow; row++) {
            if (row >= 21) break;

            HBRUSH hCurrentBrush = (row % 2 == 0) ? hBrushGray : hBrushWhite;
            COLORREF bgColor = (row % 2 == 0) ? RGB(240, 240, 240) : RGB(255, 255, 255);

            RECT rowRect = {
                startX,
                startY + (row + 1) * cellHeight,
                startX + width,
                startY + (row + 2) * cellHeight
            };

            // Preencher fundo da linha inteira
            FillRect(hdcMem, &rowRect, hCurrentBrush);

            int xPosLabel = startX + 10;
            int yPosLabel = startY + (row + 1) * cellHeight + 7;

            fonte(L"Header", RGB(0, 0, 0), hdcMem);

            // CORREÇÃO: Configurar cor de fundo para combinar com a linha
            SetBkColor(hdcMem, bgColor);

            // Desenhar labels
            const wchar_t* labels[] = {
                L"Nome do Pet:", L"Raça:", L"Nome do Tutor:", L"CEP:", L"Cor:",
                L"Idade:", L"Peso:", L"Sexo:", L"Castrado:", L"Endereço:",
                L"Ponto de Referência:", L"Banho:", L"Tosa:", L"Observação:",
                L"Parasitas:", L"Lesões:", L"Observação:", L"Telefone:", L"CPF:",
                L"Data:", L"Hora:"
            };

            if (row < 21) {
                TextOut(hdcMem, xPosLabel, yPosLabel, labels[row], wcslen(labels[row]));
            }
        }

        // Limpar recursos
        DeleteObject(hBrushWhite);
        DeleteObject(hBrushGray);

        // Copiar buffer para tela
        BitBlt(hdc, 0, 0, g_clientWidth, g_clientHeight, hdcMem, 0, 0, SRCCOPY);

        // Limpar recursos do buffer
        SelectObject(hdcMem, hOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        HWND hCtrl = (HWND)lParam;

        // Verificar se o controle é um radio button
        if (GetWindowLongPtr(hCtrl, GWL_STYLE) & BS_AUTORADIOBUTTON) {
            SetBkMode(hdc, TRANSPARENT); // Definir fundo transparente
            return (INT_PTR)hBrushTransparent; // Retornar o pincel transparente
        }
        break;
    }

    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT); // Fundo transparente para botões push
        return (INT_PTR)hBrushTransparent;
    }

    case WM_DESTROY:
    {
        // Limpar array de controles
        g_editControls.clear();
        g_hButton = NULL;
        windowClose(hWnd, message, wParam, lParam);
    }
    break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
LRESULT CALLBACK NewWndProcEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc, 10, 10, L"Esta é a nova janela Edit!", 21);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
BOOL InitEdit(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProcEdit;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"EditClass";
    return RegisterClassW(&wc);
}