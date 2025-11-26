#include "PetsAdd.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "PetsFuncoes.h"
#include <string>
#include "sqlite3.h"
#include <vector>
#include <uxtheme.h>
#include <regex>
#include <cctype>
#pragma comment(lib, "uxtheme.lib")

// IDs dos controles
#define ID_RADIO_PADRAO 20001
#define ID_RADIO_HIDRATACAO 20002
#define ID_RADIO_BANHO_NENHUM 20003

#define ID_RADIO_TESOURA 20004
#define ID_RADIO_MAQUINA 20005
#define ID_RADIO_HIGIENICA 20006
#define ID_RADIO_TOSADARACA 20007
#define ID_RADIO_TOSA_NENHUM 20008

#define ID_CHECKBOX_PULGAS 20009
#define ID_CHECKBOX_CARRAPATOS 20010

#define ID_CHECKBOX_PELE 20011
#define ID_CHECKBOX_OLHOS 20012
#define ID_CHECKBOX_SECRECAO 20013
#define ID_CHECKBOX_OUVIDO 20014

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcPetsAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para registrar a classe da janela
BOOL RegisterPetsAddClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcPetsAdd;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"JanelaPetsAddClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Add COM SCROLL CORRIGIDO
LRESULT CALLBACK WndProcPetsAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Processar o menu APENAS para mensagens específicas
    if (message == WM_COMMAND || message == WM_INITMENU || message == WM_MENUSELECT) {
        if (PetsSelect_ProcessarMenu(hWnd, message, wParam, lParam)) {
            return 0; // Mensagem já processada pelo menu
        }
    }

    // Depois processa as mensagens específicas da janela
    switch (message)
    {
    case WM_CREATE:
    {
        // Resetar scroll para garantir que comece do topo
        PetsSelect_g_scrollY = 0;
        PetsSelect_g_scrollX = 0;

        // Limpar array de controles
        PetsSelect_g_editControls.clear();

        PetsSelect_ConfigurarScrollBarsAgendamento(hWnd);

        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;

        // Configurar a tabela com scroll
        int cellHeight = 32;
        int numColumns = 21;
        int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
        int startY = 40 - PetsSelect_g_scrollY;
        int startX = 22 - PetsSelect_g_scrollX;

        // Criar campos de entrada
        for (int col = 0; col < 8; col++) {
            int colNumber = col + 1;
            int controlID = col + 2; // IDs de 2 a 22
            int xPos = startX + cellWidth + 10;
            int yPos = startY + colNumber * cellHeight + 3;

            if (col == 7) {
                HWND hCheckbox = CreateWindowW(
                    L"BUTTON",                       // Classe do controle
                    NULL,    // Texto da checkbox
                    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,  // Estilos IMPORTANTES
                    xPos, yPos,                      // Posição X, Y
                    20, 20,                         // Largura, Altura  
                    hWnd,                            // Janela pai
                    (HMENU)(controlID),              // ID único
                    NULL,                       // Instância
                    NULL
                );
                PetsSelect_g_editControls.push_back(hCheckbox);
            }
            else if (col == 2) {
                HWND hComboBox = CreateWindowW(
                    L"COMBOBOX",                         // Classe do controle: MUDAR de "BUTTON" para "COMBOBOX"
                    NULL,                                // Texto: NULL para ComboBox
                    WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, // Estilos IMPORTANTES
                    xPos, yPos,                          // Posição X, Y
                    200, 200,                            // Largura, Altura (A altura precisa ser maior para exibir a lista)
                    hWnd,                                // Janela pai
                    (HMENU)(controlID),                  // ID único
                    NULL,                                // Instância
                    NULL
                );

                PetsSelect_g_editControls.push_back(hComboBox);

                // Criar botão consultar
                PetsSelect_g_hButton_consultar = CreateWindowW(
                    L"BUTTON", L"Consultar",
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
                    xPos + 700, yPos, 80, 25,
                    hWnd, (HMENU)(0),
                    (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
                );
            }
            else if (col == 6) {
                HWND hComboBox = CreateWindowW(
                    L"COMBOBOX",                         // Classe do controle: MUDAR de "BUTTON" para "COMBOBOX"
                    NULL,                                // Texto: NULL para ComboBox
                    WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, // Estilos IMPORTANTES
                    xPos, yPos,                          // Posição X, Y
                    200, 200,                            // Largura, Altura (A altura precisa ser maior para exibir a lista)
                    hWnd,                                // Janela pai
                    (HMENU)(controlID),                  // ID único
                    NULL,                                // Instância
                    NULL
                );

                SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
                SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Macho");
                SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Fêmea");

                PetsSelect_g_editControls.push_back(hComboBox);
            }
            else {
                HWND hEdit = CreateWindowEx(
                    0, L"EDIT", L"",
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                    xPos, yPos, 200, 25, hWnd, (HMENU)(controlID), NULL, NULL
                );
                PetsSelect_g_editControls.push_back(hEdit);
            }
        }

        // Criar botão
        int buttonY = startY + 9 * cellHeight + 3;
        PetsSelect_g_hButton = CreateWindowW(
            L"BUTTON", L"Salvar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
            startX, buttonY, 150, 30,
            hWnd, (HMENU)(1),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        break;
    }
    case WM_GETDLGCODE:
        // Permitir que a janela processe a tecla Tab para navegação
        if (wParam == VK_TAB) {
            return DLGC_WANTTAB;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;

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
            PetsSelect_g_scrollY = si.nPos;
            SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
            // Atualizar posições dos controles ANTES do redraw
            PetsSelect_AtualizarPosicoesControlesAgendamento(hWnd);
            SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
            PetsSelect_invalidateDrawing(hWnd);
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
            PetsSelect_g_scrollX = si.nPos;

            SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
            // Atualizar posições dos controles ANTES do redraw
            PetsSelect_AtualizarPosicoesControlesAgendamento(hWnd);
            SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
            PetsSelect_invalidateDrawing(hWnd);
            UpdateWindow(hWnd);
        }
        break;
    }

    case WM_SIZE: {
        int newWidth = LOWORD(lParam);
        int newHeight = HIWORD(lParam);

        PetsSelect_g_clientWidth = newWidth;
        PetsSelect_g_clientHeight = newHeight;

        SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
        PetsSelect_ConfigurarScrollBarsAgendamento(hWnd);
        // Atualizar posições dos controles após redimensionamento
        PetsSelect_AtualizarPosicoesControlesAgendamento(hWnd);
        SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
        PetsSelect_invalidateDrawing(hWnd);
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
            PetsSelect_g_scrollY = si.nPos;
            // Atualizar posições dos controles ANTES do redraw
            SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
            PetsSelect_AtualizarPosicoesControlesAgendamento(hWnd);
            SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
            PetsSelect_invalidateDrawing(hWnd);
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
            std::wstring dados[12];
            for (int i = 2; i <= 11; i++) {
                std::wstring controlIDStr = std::to_wstring(i);
                HWND input = GetDlgItem(hWnd, i);

                if (i == 9) {
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
                else if (i == 4) {
                    if (input) {
                        LRESULT selectedIndex = 0;
                        LRESULT idTutorSelecionado = 0; // Armazenará o ID do Tutor

                        // 1. OBTÉM O ÍNDICE DO ITEM SELECIONADO
                        selectedIndex = SendMessage(
                            input,              // HWND do ComboBox
                            CB_GETCURSEL,       // Mensagem para obter o índice do item selecionado
                            0, 0
                        );

                        // 2. VERIFICA SE ALGO FOI SELECIONADO
                        if (selectedIndex != CB_ERR) {

                            // 3. USA O ÍNDICE PARA OBTER OS DADOS ANEXADOS (O ID do Tutor)
                            idTutorSelecionado = SendMessage(
                                input,                  // HWND do ComboBox
                                CB_GETITEMDATA,         // Mensagem para obter os dados (LPARAM) anexados
                                (WPARAM)selectedIndex,  // wParam: O índice do item
                                0
                            );

                            // 4. VERIFICA SE O DADO (ID) É VÁLIDO
                            if (idTutorSelecionado != CB_ERR) {

                                // Verifica se o ID é 0 (que geralmente é seu item 'vazio' no índice 0)
                                if (idTutorSelecionado != 0) {
                                    // Converte o ID (LRESULT) para wstring e armazena em dados[i]
                                    // Note: Fazendo cast para long long para garantir a conversão segura.
                                    dados[i] = std::to_wstring((long long)idTutorSelecionado);
                                }
                                else {
                                    // O item selecionado é o item 'vazio' (ID=0), então armazena string vazia.
                                    dados[i] = L"";
                                }
                            }
                            else {
                                // O item selecionado (por algum erro) não tinha dados anexados
                                dados[i] = L"";
                            }

                        }
                        else {
                            // Nada está selecionado no ComboBox
                            dados[i] = L"";
                        }
                    }
                }
                else if (i == 8) {
                    if (input) {
                        LRESULT selectedIndex = 0;

                        // 1. Envia a mensagem CB_GETCURSEL (Get Current Selection) para obter o índice.
                        selectedIndex = SendMessage(
                            input,
                            CB_GETCURSEL,
                            0,
                            0
                        );

                        // Verifica se a seleção é válida (índice 0 ou maior)
                        if (selectedIndex == CB_ERR) // CB_ERR (-1) é retornado se não houver seleção ou erro.
                        {
                            // Se a seleção for inválida ou inexistente, armazena string vazia.
                            dados[i] = L"";
                            continue;
                        }

                        // 2. Envia a mensagem CB_GETLBTEXTLEN (Get ListBox Text Length) para obter o tamanho do texto.
                        // O resultado é o tamanho da string em caracteres (sem incluir o terminador NULL).
                        LRESULT textLength = SendMessage(
                            input,
                            CB_GETLBTEXTLEN,
                            (WPARAM)selectedIndex, // wParam: o índice do item
                            0                      // lParam: Não usado
                        );

                        if (textLength == CB_ERR)
                        {
                            // Erro ao obter o tamanho do texto.
                            dados[i] = L"";
                            continue;
                        }

                        // 3. Cria uma std::wstring com o tamanho exato + 1 (para o terminador NULL).
                        // O resize() garante que o buffer interno seja grande o suficiente.
                        std::wstring selectedText;
                        selectedText.resize(textLength + 1); // +1 para o terminador NULL

                        // 4. Envia a mensagem CB_GETLBTEXT (Get ListBox Text) para copiar o texto.
                        // O LPARAM é o ponteiro para o buffer (wchar_t*).
                        LRESULT result = SendMessage(
                            input,
                            CB_GETLBTEXT,
                            (WPARAM)selectedIndex,           // wParam: o índice do item
                            (LPARAM)selectedText.data()      // lParam: Ponteiro para o buffer de caracteres (wchar_t*)
                        );

                        if (result == CB_ERR)
                        {
                            // Erro ao copiar o texto.
                            dados[i] = L"";
                            continue;
                        }

                        // 5. Finalização: O texto real copiado exclui o terminador NULL, 
                        // então é necessário ajustar o tamanho da wstring.
                        // O 'result' de CB_GETLBTEXT é o número de caracteres copiados (textLength).
                        selectedText.resize(result);

                        // 6. Armazena o texto no seu vetor de dados.
                        dados[i] = selectedText;
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

            // Abrir ou criar o banco de dados (código original mantido)
            sqlite3* db = nullptr;
            char* errMsg = nullptr;
            int rc;
            OpenDatabase(db);

            if (!OpenDatabase(db)) {
                MessageBox(hWnd, L"Erro ao abrir/criar o banco de dados!", L"Erro", MB_OK | MB_ICONERROR);
            }
            else {
                std::wstring currentDate = PetsSelect_GetCurrentDate();
                std::wstring currentHour = PetsSelect_GetCurrentHour();

                std::wstring sqlInsertW = L"INSERT INTO Pets (Nome_do_Pet, Raca, ID_Tutor_FK, Cor, Idade, Peso, Sexo, Castrado, Date, Hour) VALUES ('" + PetsSelect_treatDataAppointment(dados[2], 2) + L"', '" + PetsSelect_treatDataAppointment(dados[3], 3) + L"', '" + PetsSelect_treatDataAppointment(dados[4], 4) + L"', '" + PetsSelect_treatDataAppointment(dados[5], 5) + L"', '" + PetsSelect_treatDataAppointment(dados[6], 6) + L"', '" + PetsSelect_treatDataAppointment(dados[7], 7) + L"', '" + PetsSelect_treatDataAppointment(dados[8], 8) + L"', '" + PetsSelect_treatDataAppointment(dados[9], 9) + L"', '" + currentDate + L"', '" + currentHour + L"');";

                if (PetsSelect_error == L"1") {
                    MessageBox(hWnd, PetsSelect_msg, L"Erro", MB_OK | MB_ICONERROR);
                    PetsSelect_error = L"0";
                    PetsSelect_mensagem.clear();
                }
                else {
                    std::string sqlInsertUtf8 = PetsSelect_WideToUTF8(sqlInsertW);
                    errMsg = nullptr;
                    rc = sqlite3_exec(db, sqlInsertUtf8.c_str(), nullptr, nullptr, &errMsg);

                    if (rc != SQLITE_OK && errMsg) {
                        std::wstring wErrMsg = PetsSelect_UTF8ToWide(errMsg);
                        MessageBox(hWnd, wErrMsg.c_str(), L"Erro", MB_OK | MB_ICONERROR);
                        sqlite3_free(errMsg);
                    }
                    else {
                        AtualizarJanelas();
                        MessageBox(hWnd, L"Dados inseridos com sucesso!", L"Sucesso", MB_OK);
                    }
                }
                sqlite3_close(db);
            }
        }
        else if (wmId == 0) { //CONSULTAR
            HWND input = GetDlgItem(hWnd, 4);
            LRESULT selectedIndex = SendMessage(input, CB_GETCURSEL, 0, 0);

            // 2. Envia a mensagem CB_GETCURSEL (Get Current Selection) para o ComboBox.
            //    O resultado (o índice do item selecionado) é retornado no LRESULT.
            LRESULT id = SendMessage(
                input,          // HWND do ComboBox
                CB_GETITEMDATA,   // Mensagem para obter o índice do item selecionado
                (WPARAM)selectedIndex,              // wParam: Não usado (deve ser 0)
                0               // lParam: Não usado (deve ser 0)
            );
            
            if (id == 0) {
                MessageBox(hWnd, L"Selecione o Tutor!", L"Erro", MB_OK | MB_ICONERROR);
                break;
            }

            TutoresSelect_idRecord = id;

            if (!PetsSelect_CreateNewWindow(hWnd, hInst, L"JanelaTutoresReadClasse", L"DADOS DO CLIENTE"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaTutoresReadClasse", NULL);
                if (hWndRead != NULL)
                {
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                    SetForegroundWindow(hWndRead);
                }
            }
        }
        break;
    }

    case WM_PAINT:
    {

        TutoresSelect_Global_selectDB();

        HWND hComboBox = PetsSelect_g_editControls[2];
        SendMessage(hComboBox, CB_RESETCONTENT, 0, 0);
        TutoresSelect_Global_preencherComboBox(hComboBox);

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Double buffering
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, PetsSelect_g_clientWidth, PetsSelect_g_clientHeight);
        HGDIOBJ hOld = SelectObject(hdcMem, hbmMem);

        // Preencher fundo completo com branco
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        FillRect(hdcMem, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));

        PetsSelect_fonte(L"Font", RGB(0, 0, 0), hdcMem);

        int width = PetsSelect_g_clientWidth - 44;
        int cellHeight = 32;
        int startY = 40 - PetsSelect_g_scrollY;
        int startX = 22 - PetsSelect_g_scrollX;

        // Título com fundo branco
        RECT titleRect = { startX - 5, startY - 25, startX + 250, startY + 5 };
        FillRect(hdcMem, &titleRect, (HBRUSH)(COLOR_WINDOW + 1));
        PetsSelect_windowsTitle(hdcMem, startX, startY - 20, L"CADASTRO DO PET", 15);

        // Desenhar linhas visíveis
        int firstVisibleRow = max(0, (PetsSelect_g_scrollY - 40) / cellHeight);
        int lastVisibleRow = min(7, firstVisibleRow + (PetsSelect_g_clientHeight / cellHeight) + 2);

        HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));

        // CORREÇÃO: Usar background OPAQUE com cores consistentes
        SetBkMode(hdcMem, OPAQUE);

        for (int row = firstVisibleRow; row <= lastVisibleRow; row++) {
            if (row >= 8) break;

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

            PetsSelect_fonte(L"Header", RGB(0, 0, 0), hdcMem);

            // CORREÇÃO: Configurar cor de fundo para combinar com a linha
            SetBkColor(hdcMem, bgColor);

            // Desenhar labels
            const wchar_t* labels[] = {
                L"Nome do Pet:", L"Raça:", L"Nome do Tutor:", L"Cor:",
                L"Idade:", L"Peso:", L"Sexo:", L"Castrado:"
            };

            if (row < 8) {
                TextOut(hdcMem, xPosLabel, yPosLabel, labels[row], wcslen(labels[row]));
            }
        }

        // Limpar recursos
        DeleteObject(hBrushWhite);
        DeleteObject(hBrushGray);

        // Copiar buffer para tela
        BitBlt(hdc, 0, 0, PetsSelect_g_clientWidth, PetsSelect_g_clientHeight, hdcMem, 0, 0, SRCCOPY);

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
            return (INT_PTR)PetsSelect_hBrushTransparent; // Retornar o pincel transparente
        }
        break;
    }

    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT); // Fundo transparente para botões push
        return (INT_PTR)PetsSelect_hBrushTransparent;
    }

    case WM_DESTROY:
    {
        // Limpar array de controles
        PetsSelect_g_editControls.clear();
        PetsSelect_g_hButton = NULL;
        PetsSelect_windowClose(hWnd, message, wParam, lParam);
    }
    break;
    case WM_LBUTTONDOWN:
    {
        if (PetsSelect_g_isRedrawing)
        {
            return 0; // Ignora o clique durante o redesenho
        }
        // Lógica existente para clique, se aplicável
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

    case WM_LBUTTONUP:
    {
        if (PetsSelect_g_isRedrawing)
        {
            return 0; // Ignora o clique durante o redesenho
        }
        // Lógica existente para clique, se aplicável
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    case WM_KEYDOWN:
    {
        PetsSelect_Shortcuts(hWnd, message, wParam, lParam);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
LRESULT CALLBACK NewWndProcPetsAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        PetsSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
BOOL InitPetsAdd(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProcPetsAdd;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"AddClass";
    return RegisterClassW(&wc);
}