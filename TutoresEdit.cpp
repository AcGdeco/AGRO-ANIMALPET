#include "TutoresEdit.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "TutoresFuncoes.h"
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

std::vector<std::vector<std::wstring>> TutoresSelect_g_tableDataEditar;

void TutoresSelect_PreencherControlesEdicao(HWND hWnd) {
    // 1. Busca os Dados no Banco de Dados
    TutoresSelect_selectBD();
    TutoresSelect_cpf = TutoresSelect_g_tableDataEditar[1][6];

    // Se não houver dados, retorna
    if (TutoresSelect_g_tableDataEditar.size() <= 1) {
        return;
    }

    // 2. Preenchimento dos Campos de Entrada
    // O loop deve corresponder ao loop de criação para garantir que 
    // os dados e os controles (controlID = col + 2) estejam sincronizados.
    for (int col = 0; col < 6; col++) {
        int controlID = col + 2;
        std::wstring displayText = TutoresSelect_g_tableDataEditar[1][col + 1];

        // 3. Obter o HWND do controle (pelo ID ou pelo vetor g_editControls)
        // Usar GetDlgItem(hWnd, controlID) ou iterar sobre g_editControls
        HWND hControl = GetDlgItem(hWnd, controlID);

        if (hControl == NULL) {
            // Se hControl é NULL, é porque ele é um Radio Button (que tem IDs diferentes)
            // ou é um dos outros Checkboxes.
            // Para os Checkboxes e Edit Controls, o hControl não será NULL aqui.
        }
        else {
            SetWindowText(hControl, displayText.c_str());
        }
    }
}

void TutoresSelect_CriarControlesEdicao(HWND hWnd) {
    // 1. Resetar Scroll e Limpar Controles Antigos
    TutoresSelect_g_scrollY = 0;
    TutoresSelect_g_scrollX = 0;

    // É crucial DESTRUIR os HWNDs antes de limpar o vetor,
    // para evitar vazamento de recursos.
    // Assumindo que você tem uma função para isso:
    // TutoresSelect_DestroyAllControls(); 
    TutoresSelect_g_editControls.clear();

    TutoresSelect_ConfigurarScrollBarsAgendamento(hWnd);

    // 2. Cálculo de Dimensões
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;

    int cellHeight = 32;
    int numColumns = 21;
    int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);

    // As posições iniciais dependem do scroll, mas aqui elas são 0
    // pois os scrollbars acabaram de ser resetados.
    int startY = 40; // O scroll será aplicado na função de atualização de posição
    int startX = 22;

    // 3. Criação dos Campos de Entrada
    for (int col = 0; col < 6; col++) {
        int colNumber = col + 1;
        int controlID = col + 2; // IDs de 2 a 22
        int xPos = startX + cellWidth + 10;
        int yPos = startY + colNumber * cellHeight + 3;

        HWND hEdit = CreateWindowEx(
            0, L"EDIT", L"", // Cria vazio
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
            xPos, yPos, 200, 25, hWnd, (HMENU)(controlID), NULL, NULL
        );
        TutoresSelect_g_editControls.push_back(hEdit);
    }

    // 4. Criação do Botão Salvar
    int buttonY = startY + 7 * cellHeight + 3;
    TutoresSelect_g_hButton = CreateWindowW(
        L"BUTTON", L"Salvar",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
        startX, buttonY, 150, 30,
        hWnd, (HMENU)(1),
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
    );
}

void TutoresSelect_selectBD() {
    // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
    TutoresSelect_g_tableDataEditar.clear();

    // Consultar o banco apenas se a tabela estiver vazia
    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("pet.db", &db);
    if (rc == SQLITE_OK) {
        std::string idRecordStr = std::to_string(TutoresSelect_idRecord);
        std::string sqlSelect = "SELECT * FROM Tutores WHERE ID = '" + idRecordStr + "';";

        rc = sqlite3_exec(db, sqlSelect.c_str(), TutoresSelect_sqlite_callback, &TutoresSelect_g_tableDataEditar, &errMsg);
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
                TutoresSelect_g_tableDataEditar.push_back({ L"Erro", wErrMsg });
                sqlite3_free(errMsg);
            }
            else {
                TutoresSelect_g_tableDataEditar.push_back({ L"Erro", L"Erro desconhecido no SQLite: " + std::to_wstring(rc) });
            }
        }
        else {
            // Sucesso - talvez adicionar uma mensagem de confirmação
            if (TutoresSelect_g_tableDataEditar.empty()) {
                TutoresSelect_g_tableDataEditar.push_back({ L"Info", L"Nenhum registro encontrado com ID: " + std::to_wstring(TutoresSelect_idRecord) });
            }
        }

        sqlite3_close(db);
    }
    else {
        TutoresSelect_g_tableDataEditar.push_back({ L"Erro", L"Não foi possível abrir o banco" });
    }
}

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcTutoresEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para registrar a classe da janela
BOOL RegisterTutoresEditClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcTutoresEdit;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"JanelaTutoresEditClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Edit COM SCROLL CORRIGIDO
LRESULT CALLBACK WndProcTutoresEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Processar o menu APENAS para mensagens específicas
    if (message == WM_COMMAND || message == WM_INITMENU || message == WM_MENUSELECT) {
        if (TutoresSelect_ProcessarMenu(hWnd, message, wParam, lParam)) {
            return 0; // Mensagem já processada pelo menu
        }
    }

    // Depois processa as mensagens específicas da janela
    switch (message)
    {
    case WM_CREATE:
    {
        // 1. Cria a interface
        TutoresSelect_CriarControlesEdicao(hWnd);
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
            TutoresSelect_g_scrollY = si.nPos;
            SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
            // Atualizar posições dos controles ANTES do redraw
            TutoresSelect_AtualizarPosicoesControlesAgendamento(hWnd);
            SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
            TutoresSelect_invalidateDrawing(hWnd);
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
            TutoresSelect_g_scrollX = si.nPos;
            SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
            // Atualizar posições dos controles ANTES do redraw
            TutoresSelect_AtualizarPosicoesControlesAgendamento(hWnd);
            SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
            TutoresSelect_invalidateDrawing(hWnd);
            UpdateWindow(hWnd);
        }
        break;
    }

    case WM_SIZE: {
        int newWidth = LOWORD(lParam);
        int newHeight = HIWORD(lParam);

        TutoresSelect_g_clientWidth = newWidth;
        TutoresSelect_g_clientHeight = newHeight;

        SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
        TutoresSelect_ConfigurarScrollBarsAgendamento(hWnd);
        // Atualizar posições dos controles após redimensionamento
        TutoresSelect_AtualizarPosicoesControlesAgendamento(hWnd);
        SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
        TutoresSelect_invalidateDrawing(hWnd);
        UpdateWindow(hWnd);
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
            TutoresSelect_g_scrollY = si.nPos;
            SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
            // Atualizar posições dos controles ANTES do redraw
            TutoresSelect_AtualizarPosicoesControlesAgendamento(hWnd);
            SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
            TutoresSelect_invalidateDrawing(hWnd);
            UpdateWindow(hWnd);
        }
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        if (TutoresSelect_g_isRedrawing)
        {
            return 0; // Ignora o clique durante o redesenho
        }
        // Lógica existente para clique, se aplicável
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

    case WM_LBUTTONUP:
    {
        if (TutoresSelect_g_isRedrawing)
        {
            return 0; // Ignora o clique durante o redesenho
        }
        // Lógica existente para clique, se aplicável
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
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
            std::wstring dados[8];
            for (int i = 2; i <= 7; i++) {
                std::wstring controlIDStr = std::to_wstring(i);
                HWND input = GetDlgItem(hWnd, i);
                
                if (input) {
                    wchar_t buffer[256];
                    GetWindowText(input, buffer, 256);
                    dados[i] = std::wstring(buffer);
                }
            }

            sqlite3* db;
            char* errMsg = 0;
            int rc = sqlite3_open("pet.db", &db);
            if (rc) {
                MessageBox(hWnd, L"Erro ao abrir/criar o banco de dados!", L"Erro", MB_OK | MB_ICONERROR);
            }
            else {
                std::wstring currentDate = TutoresSelect_GetCurrentDate();
                std::wstring currentHour = TutoresSelect_GetCurrentHour();

                std::wstring sqlInsertW = L"UPDATE Tutores SET "
                    L"Nome_do_Tutor = '" + TutoresSelect_treatDataAppointment(dados[2], 2) + L"', "
                    L"CEP = '" + TutoresSelect_treatDataAppointment(dados[3], 3) + L"', "
                    L"Endereco = '" + TutoresSelect_treatDataAppointment(dados[4], 4) + L"', "
                    L"Ponto_de_referencia = '" + TutoresSelect_treatDataAppointment(dados[5], 5) + L"', "
                    L"Telefone = '" + TutoresSelect_treatDataAppointment(dados[6], 6) + L"', "
                    L"CPF = '" + TutoresSelect_treatDataAppointment(dados[7], 7) + L"', "
                    L"Date = '" + currentDate + L"', "
                    L"Hour = '" + currentHour + L"' "
                    L"WHERE ID = " + std::to_wstring(TutoresSelect_idRecord) + L";";

                if (TutoresSelect_error == L"1") {
                    MessageBox(hWnd, TutoresSelect_msg, L"Erro", MB_OK | MB_ICONERROR);
                    TutoresSelect_error = L"0";
                    TutoresSelect_mensagem.clear();
                }
                else {
                    std::string sqlInsertUtf8 = TutoresSelect_WideToUTF8(sqlInsertW);
                    errMsg = nullptr;
                    rc = sqlite3_exec(db, sqlInsertUtf8.c_str(), nullptr, nullptr, &errMsg);

                    if (rc != SQLITE_OK && errMsg) {
                        std::wstring wErrMsg = TutoresSelect_UTF8ToWide(errMsg);
                        MessageBox(hWnd, wErrMsg.c_str(), L"Erro", MB_OK | MB_ICONERROR);
                        sqlite3_free(errMsg);
                    }
                    else {
                        HWND hwndSelect = FindWindow(TEXT("JanelaTutoresSelectClasse"), NULL);
                        if (hwndSelect != NULL) {
                            //std::cout << "Janela encontrada! HWND: " << hwnd << std::endl;
                            TutoresSelect_RecarregarDadosTabela(hwndSelect);
                        }
                        hwndSelect = FindWindow(TEXT("JanelaTutoresReadClasse"), NULL);
                        if (hwndSelect != NULL) {
                            //std::cout << "Janela encontrada! HWND: " << hwnd << std::endl;
                            TutoresSelect_invalidateDrawing(hwndSelect);
                            UpdateWindow(hwndSelect);
                        }
                        hwndSelect = FindWindow(TEXT("JanelaPetsSelectClasse"), NULL);
                        if (hwndSelect != NULL) {
                            //std::cout << "Janela encontrada! HWND: " << hwnd << std::endl;
                            PetsSelect_RecarregarDadosTabela(hwndSelect);
                        }
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

        // 2. Preenche os campos com os dados do banco de dados
        TutoresSelect_PreencherControlesEdicao(hWnd);

        // Double buffering
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, TutoresSelect_g_clientWidth, TutoresSelect_g_clientHeight);
        HGDIOBJ hOld = SelectObject(hdcMem, hbmMem);

        // Preencher fundo completo com branco
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        FillRect(hdcMem, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));

        TutoresSelect_fonte(L"Font", RGB(0, 0, 0), hdcMem);

        int width = TutoresSelect_g_clientWidth - 44;
        int cellHeight = 32;
        int startY = 40 - TutoresSelect_g_scrollY;
        int startX = 22 - TutoresSelect_g_scrollX;

        // Título com fundo branco
        RECT titleRect = { startX - 5, startY - 25, startX + 250, startY + 5 };
        FillRect(hdcMem, &titleRect, (HBRUSH)(COLOR_WINDOW + 1));
        TutoresSelect_windowsTitle(hdcMem, startX, startY - 20, L"EDITAR TUTOR", 12);

        // Desenhar linhas visíveis
        int firstVisibleRow = max(0, (TutoresSelect_g_scrollY - 40) / cellHeight);
        int lastVisibleRow = min(5, firstVisibleRow + (TutoresSelect_g_clientHeight / cellHeight) + 2);

        HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));

        // CORREÇÃO: Usar background OPAQUE com cores consistentes
        SetBkMode(hdcMem, OPAQUE);

        for (int row = firstVisibleRow; row <= lastVisibleRow; row++) {
            if (row >= 6) break;

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

            TutoresSelect_fonte(L"Header", RGB(0, 0, 0), hdcMem);

            // CORREÇÃO: Configurar cor de fundo para combinar com a linha
            SetBkColor(hdcMem, bgColor);

            // Desenhar labels
            const wchar_t* labels[] = {
                L"Nome do Tutor:", L"CEP:", L"Endereço:",
                L"Ponto de Referência:", L"Telefone:", L"CPF:"
            };

            if (row < 6) {
                TextOut(hdcMem, xPosLabel, yPosLabel, labels[row], wcslen(labels[row]));
            }
        }

        // Limpar recursos
        DeleteObject(hBrushWhite);
        DeleteObject(hBrushGray);

        // Copiar buffer para tela
        BitBlt(hdc, 0, 0, TutoresSelect_g_clientWidth, TutoresSelect_g_clientHeight, hdcMem, 0, 0, SRCCOPY);

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
            return (INT_PTR)TutoresSelect_hBrushTransparent; // Retornar o pincel transparente
        }
        break;
    }

    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT); // Fundo transparente para botões push
        return (INT_PTR)TutoresSelect_hBrushTransparent;
    }

    case WM_DESTROY:
    {
        // Limpar array de controles
        TutoresSelect_g_editControls.clear();
        TutoresSelect_g_hButton = NULL;
        TutoresSelect_windowClose(hWnd, message, wParam, lParam);
    }
    break;
    case WM_KEYDOWN:
    {
        TutoresSelect_Shortcuts(hWnd, message, wParam, lParam);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
LRESULT CALLBACK NewWndProcTutoresEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        TutoresSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
BOOL InitTutoresEdit(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProcTutoresEdit;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"EditClass";
    return RegisterClassW(&wc);
}