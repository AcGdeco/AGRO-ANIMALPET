#include "Add.h"
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
#define ID_RADIO_PADRAO 2001
#define ID_RADIO_HIDRATACAO 2002
#define ID_RADIO_BANHO_NENHUM 2003

#define ID_RADIO_TESOURA 2004
#define ID_RADIO_MAQUINA 2005
#define ID_RADIO_HIGIENICA 2006
#define ID_RADIO_TOSADARACA 2007
#define ID_RADIO_TOSA_NENHUM 2008

#define ID_CHECKBOX_PULGAS 2009
#define ID_CHECKBOX_CARRAPATOS 2010

#define ID_CHECKBOX_PELE 2011
#define ID_CHECKBOX_OLHOS 2012
#define ID_CHECKBOX_SECRECAO 2013
#define ID_CHECKBOX_OUVIDO 2014

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para registrar a classe da janela
BOOL RegisterAddClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcAdd;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"JanelaAddClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Add COM SCROLL CORRIGIDO
LRESULT CALLBACK WndProcAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
                    (HMENU)(ID_RADIO_PADRAO),       // ID único
                    NULL,                            // Instância
                    NULL
                );
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
                    (HMENU)(ID_RADIO_HIDRATACAO),       // ID único
                    NULL,                            // Instância
                    NULL
                );
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
                    (HMENU)(ID_RADIO_BANHO_NENHUM),       // ID único
                    NULL,                            // Instância
                    NULL
                );
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
                    (HMENU)(ID_RADIO_TESOURA),       // ID único
                    NULL,                            // Instância
                    NULL
                );
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
                    (HMENU)(ID_RADIO_MAQUINA),       // ID único
                    NULL,                            // Instância
                    NULL
                );
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
                    (HMENU)(ID_RADIO_HIGIENICA),       // ID único
                    NULL,                            // Instância
                    NULL
                );
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
                    (HMENU)(ID_RADIO_TOSADARACA),       // ID único
                    NULL,                            // Instância
                    NULL
                );
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
                    (HMENU)(ID_RADIO_TOSA_NENHUM),       // ID único
                    NULL,                            // Instância
                    NULL
                );
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
                    (HMENU)(ID_CHECKBOX_PULGAS),              // ID único
                    NULL,                       // Instância
                    NULL
                );
                g_editControls.push_back(hCheckbox);

                hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Carrapatos",    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(ID_CHECKBOX_CARRAPATOS),              // ID único
                    NULL,                       // Instância
                    NULL
                );
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
                    (HMENU)(ID_CHECKBOX_PELE),              // ID único
                    NULL,                       // Instância
                    NULL
                );
                g_editControls.push_back(hCheckbox);

                hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Olhos",    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(ID_CHECKBOX_OLHOS),              // ID único
                    NULL,                       // Instância
                    NULL
                );
                g_editControls.push_back(hCheckbox);

                hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Secreção",    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(ID_CHECKBOX_SECRECAO),              // ID único
                    NULL,                       // Instância
                    NULL
                );
                g_editControls.push_back(hCheckbox);

                hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    L"Ouvido",    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(ID_CHECKBOX_OUVIDO),              // ID único
                    NULL,                       // Instância
                    NULL
                );
                g_editControls.push_back(hCheckbox);
            }
            else {
                HWND hEdit = CreateWindowEx(
                    0, L"EDIT", L"",
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
                    HWND hRadio1 = GetDlgItem(hWnd, ID_RADIO_PADRAO);
                    HWND hRadio2 = GetDlgItem(hWnd, ID_RADIO_HIDRATACAO);
                    HWND hRadio3 = GetDlgItem(hWnd, ID_RADIO_BANHO_NENHUM);
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
                    HWND hRadio1 = GetDlgItem(hWnd, ID_RADIO_TESOURA);
                    HWND hRadio2 = GetDlgItem(hWnd, ID_RADIO_MAQUINA);
                    HWND hRadio3 = GetDlgItem(hWnd, ID_RADIO_HIGIENICA);
                    HWND hRadio4 = GetDlgItem(hWnd, ID_RADIO_TOSADARACA);
                    HWND hRadio5 = GetDlgItem(hWnd, ID_RADIO_TOSA_NENHUM);
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
                    HWND hCheckbox1 = GetDlgItem(hWnd, ID_CHECKBOX_PULGAS);
                    HWND hCheckbox2 = GetDlgItem(hWnd, ID_CHECKBOX_CARRAPATOS);

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
                    HWND hCheckboxPele = GetDlgItem(hWnd, ID_CHECKBOX_PELE);
                    HWND hCheckboxOlhos = GetDlgItem(hWnd, ID_CHECKBOX_OLHOS);
                    HWND hCheckboxSecrecao = GetDlgItem(hWnd, ID_CHECKBOX_SECRECAO);
                    HWND hCheckboxOuvido = GetDlgItem(hWnd, ID_CHECKBOX_OUVIDO);

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
                        dados[i] = L""; // ou L"", dependendo do que você preferir
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

                std::wstring sqlInsertW = L"INSERT INTO Pets (Nome_do_Pet, Raca, Nome_do_Tutor, CEP, Cor, Idade, Peso, Sexo, Castrado, Endereco, Ponto_de_referencia, Banho, Tosa, Obs_Tosa, Parasitas, Lesoes, Obs_Lesoes, Telefone, CPF, Appointment_Date, Appointment_Hour, Date, Hour) VALUES ('" + treatDataAppointment(dados[2], 2) + L"', '" + treatDataAppointment(dados[3], 3) + L"', '" + treatDataAppointment(dados[4], 4) + L"', '" + treatDataAppointment(dados[5], 5) + L"', '" + treatDataAppointment(dados[6], 6) + L"', '" + treatDataAppointment(dados[7], 7) + L"', '" + treatDataAppointment(dados[8], 8) + L"', '" + treatDataAppointment(dados[9], 9) + L"', '" + treatDataAppointment(dados[10], 10) + L"', '" + treatDataAppointment(dados[11], 11) + L"', '" + treatDataAppointment(dados[12], 12) + L"', '" + treatDataAppointment(dados[13], 13) + L"', '" + treatDataAppointment(dados[14], 14) + L"', '" + treatDataAppointment(dados[15], 15) + L"', '" + treatDataAppointment(dados[16], 16) + L"', '" + treatDataAppointment(dados[17], 17) + L"', '" + treatDataAppointment(dados[18], 18) + L"', '" + treatDataAppointment(dados[19], 19) + L"', '" + treatDataAppointment(dados[20], 20) + L"', '" + treatDataAppointment(dados[21], 21) + L"', '" + treatDataAppointment(dados[22], 22) + L"', '" + currentDate + L"', '" + currentHour + L"');";

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
        windowsTitle(hdcMem, startX, startY - 20, L"CRIAR AGENDAMENTO", 17);

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
LRESULT CALLBACK NewWndProcAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc, 10, 10, L"Esta é a nova janela add!", 21);
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
BOOL InitAdd(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProcAdd;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"AddClass";
    return RegisterClassW(&wc);
}