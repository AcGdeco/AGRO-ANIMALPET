#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include <format>
#include <string>
#include "sqlite3.h"
#include <vector>
#include <cmath>
#include "TutoresFuncoes.h"

// ADICIONE estas linhas para usar as variáveis externas:
extern int TutoresSelect_g_scrollY;
extern int TutoresSelect_g_clientHeight;
extern int TutoresSelect_g_contentHeight;

bool TutoresSelect_g_wasInactive = false; // Inicialização
HWND TutoresSelect_g_hWndMain = NULL;     // Inicialização

std::vector<int> TutoresSelect_naoDesenhar;

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcTutoresSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função auxiliar para converter de UTF-8 (char*) para std::wstring (UTF-16)
std::wstring TutoresSelect_utf8_to_wstring(const char* str) {
    if (!str) return L"NULL";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstr(size_needed - 1, 0); // -1 para não incluir o caractere nulo
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size_needed);
    return wstr;
}

// Função para registrar a classe da janela (pode ser chamada de outro lugar, como Pet.cpp)
BOOL RegisterTutoresSelectClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcTutoresSelect;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;  // Menu será definido dinamicamente
    wcex.lpszClassName = L"JanelaTutoresSelectClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Select
LRESULT CALLBACK WndProcTutoresSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        TutoresSelect_g_wasInactive = false;
        TutoresSelect_g_hWndMain = hWnd;

        // Garantir que a janela tenha WS_VSCROLL
        LONG style = GetWindowLongPtr(hWnd, GWL_STYLE);
        if (!(style & WS_VSCROLL)) {
            SetWindowLongPtr(hWnd, GWL_STYLE, style | WS_VSCROLL);
        }

        TutoresSelect_RecarregarDadosTabela(hWnd);

        return 0;
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        HWND hButton = (HWND)lParam; // Handle do botão que disparou o evento
        LONG_PTR id = GetWindowLongPtr(hButton, GWLP_USERDATA); // Recuperar o id do registro

        if (wmId == TutoresSelect_CONSULTAR) // Botões "Consultar"
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Consultar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            TutoresSelect_idRecord = id;

            if (!TutoresSelect_CreateNewWindow(hWnd, hInst, L"JanelaTutoresReadClasse", L"DADOS DO CLIENTE"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaTutoresReadClasse", NULL);
                if (hWndRead != NULL)
                {
                    TutoresSelect_invalidateDrawing(hWndRead);
                    UpdateWindow(hWndRead);
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                }
            }
        }
        else if (wmId == TutoresSelect_EDITAR) // Botões "Editar"
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Editar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            TutoresSelect_idRecord = id;

            if (!TutoresSelect_CreateNewWindow(hWnd, hInst, L"JanelaTutoresEditClasse", L"EDITAR TUTOR"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaTutoresEditClasse", NULL);
                if (hWndRead != NULL)
                {
                    TutoresSelect_invalidateDrawing(hWndRead);
                    UpdateWindow(hWndRead);
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                }
            }
        }
        else if (wmId == TutoresSelect_DELETAR) // Botões "Deletar"
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Deletar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            // TutoresSelect_idRecord = id;
            std::wstring msg = L"Deletar registro ID " + std::to_wstring(id) + L"?\nPets e Agendamentos cadastrados com esse Tutor serão deletados.";
            if (MessageBoxW(hWnd, msg.c_str(), L"Confirmar", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                TutoresSelect_deleteRecordById("pet.db", id, hWnd);
                AtualizarJanelas();
            }
        }
        else if (wmId == TutoresSelect_FILTRAR)
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado!", L"Filtrar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            for (int i = 0; i < 11; i++) {
                std::wstring controlIDStr = std::to_wstring(i);
                HWND input = GetDlgItem(hWnd, i + 20);

                if (i == 7) { // Se o comando veio do nosso ComboBox
                    // 1. Obter o índice do item selecionado
                    int indiceSelecionado = (int)SendMessageW(
                        input, CB_GETCURSEL, 0, 0
                    );

                    // 2. Obter o texto do item selecionado
                    if (indiceSelecionado != CB_ERR) {
                        wchar_t buffer[256];

                        // Obter o texto do índice
                        SendMessageW(
                            input,
                            CB_GETLBTEXT,
                            (WPARAM)indiceSelecionado,
                            (LPARAM)buffer
                        );
                        TutoresSelect_dados[i] = std::wstring(buffer);

                        // O valor selecionado está em 'buffer' (ex: L"Opção B")
                        // Faça algo com o valor, como atualizar o filtro:
                        // std::wstring valorFiltro = buffer;
                        // aplicarFiltro(valorFiltro);
                    }
                }else {
                    wchar_t buffer[256];
                    GetWindowText(input, buffer, 256);
                    TutoresSelect_dados[i] = std::wstring(buffer);
                }
            }

            TutoresSelect_idNumeroUltimo = 1;
            TutoresSelect_offsetTableRow = 1;

            TutoresSelect_RecarregarDadosTabela(hWnd);
        }
        else if (wmId == TutoresSelect_ORDENAR) // Botões "Ordenar"
        {
            TutoresSelect_btnClicado = L"ORDENAR";
            TutoresSelect_idBtnGlobal = id;
            TutoresSelect_RecarregarDadosTabela(hWnd);
        }
        // Verifica se a mensagem veio do seu ComboBox LIMITAR
        else if (wmId == TutoresSelect_LIMITAR) {

            // HIWORD(wParam) é o código de notificação específico do controle
            int notificationCode = HIWORD(wParam);

            // 1. VERIFICA A MUDANÇA DE SELEÇÃO
            if (notificationCode == CBN_SELCHANGE) {

                // A SELEÇÃO MUDOU! É AQUI QUE VOCÊ CHAMA SUA FUNÇÃO.

                // Obtém o Handle do ComboBox (opcional, mas bom para clareza)
                HWND hComboBox = (HWND)lParam;

                if (!IsWindow(hComboBox)) {
                    return 0;
                }

                // CHAMAR FUNÇÃO DE AÇÃO
                TutoresSelect_handleLimitChange(hComboBox);

                TutoresSelect_RecarregarDadosTabela(hWnd);

                // Certifique-se de retornar 0 após tratar a mensagem
                return 0;
            }
        }
        else if (wmId == TutoresSelect_OFFSET) {

            TutoresSelect_mudarPagina(id);
            TutoresSelect_RecarregarDadosTabela(hWnd);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        TutoresSelect_fonte(L"Font", RGB(0, 0, 0), hdc);

        // Obter dimensões da janela
        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;
        int height = rect.bottom - rect.top;

        // Configurar a tabela
        int columnNumber = 7;
        int cellHeight = 32;
        int numColumns = TutoresSelect_g_tableData.empty() ? 0 : 7;
        int cellWidth = width / (numColumns > 0 ? numColumns + 3 : 1); // +3 para os botões
        int startY = 350 - TutoresSelect_g_scrollY;  // Posição Y com scroll
        int startX = 22 - TutoresSelect_g_scrollX;  // Posição X com scroll

        // LIMPAR a área de desenho primeiro
        HBRUSH hBgBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        FillRect(hdc, &rect, hBgBrush);
        DeleteObject(hBgBrush);

        // Desenhar fundos alternados para as linhas
        HBRUSH hBrushHeader = CreateSolidBrush(RGB(150, 150, 150));
        HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));

        // Desenhar o texto nas células
        SetBkMode(hdc, TRANSPARENT);

        //Título
        TutoresSelect_windowsTitle(hdc, startX, startY - 330, L"TUTORES", 7);

        // Desenhar filtros
        TutoresSelect_createHeaderFilters(hdc, hWnd);

        //Header Table
        TutoresSelect_createHeaderTable(hWnd, hdc);

        int linha = 1;
        int counter = 0;

        // CORREÇÃO: Cálculo seguro do limite
        int limit;
        if (TutoresSelect_rowsNumber == 0) {
            limit = 0;
        }
        else {
            limit = min(TutoresSelect_offsetTableRow + TutoresSelect_limitTableRow, TutoresSelect_rowsNumber);
        }

        // DESENHAR APENAS UMA VEZ - REMOVER loops desnecessários
        for (size_t row = TutoresSelect_offsetTableRow; row < limit && row < TutoresSelect_g_tableData.size(); row++) {
            if (row < TutoresSelect_g_tableData.size()) {
                HBRUSH hCurrentBrush = (linha % 2 == 0) ? hBrushGray : hBrushWhite;

                if (linha == 0) {
                    hCurrentBrush = hBrushHeader;
                    TutoresSelect_fonte(L"Header", RGB(255, 255, 255), hdc);
                }
                else {
                    TutoresSelect_fonte(L"Font", RGB(0, 0, 0), hdc);
                }

                // Desenhar o fundo da linha
                RECT rowRect = {
                    startX,
                    startY + static_cast<int>(linha) * cellHeight,
                    startX + width,
                    startY + (static_cast<int>(linha) + 1) * cellHeight
                };
                FillRect(hdc, &rowRect, hCurrentBrush);

                counter = 0;
                // Desenhar as células de dados
                for (size_t col = 0; col < 7; col++) {
                    std::wstring displayText = TutoresSelect_g_tableData[row][col];

                    int xPos = startX + counter * cellWidth + 10;
                    int yPos = startY + linha * cellHeight + 7;

                    int qtyCaracters = displayText.length();
                    if (width <= 1600 && displayText.length() > 15) {
                        qtyCaracters = 15;

                    }
                    else if (width <= 2000 && displayText.length() > 25) {
                        qtyCaracters = 25;
                    }
                    TextOut(hdc, xPos, yPos, displayText.c_str(), static_cast<int>(qtyCaracters));
                    counter++;
                }

                if (counter != 0) {
                    linha++;
                }
            }
        }

        // DESENHAR CABEÇALHOS DOS BOTÕES APENAS UMA VEZ - fora do loop principal
        if (!TutoresSelect_g_tableData.empty()) {
            TutoresSelect_fonte(L"Font", RGB(255, 255, 255), hdc);

            int headerY = startY + 7;

            // Consultar
            int xPos = startX + columnNumber * cellWidth + 10;
            TextOut(hdc, xPos, headerY, L"Consultar", 9);

            // Editar
            xPos = startX + (columnNumber + 1) * cellWidth + 2;
            TextOut(hdc, xPos, headerY, L"Editar", 6);

            // Deletar
            xPos = startX + (columnNumber + 2) * cellWidth + 2;
            TextOut(hdc, xPos, headerY, L"Deletar", 7);
        }

        // Limpar recursos
        DeleteObject(hBrushHeader);
        DeleteObject(hBrushWhite);
        DeleteObject(hBrushGray);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_KEYDOWN:
    {
        TutoresSelect_Shortcuts(hWnd, message, wParam, lParam);
        break;
    }

    case WM_DESTROY:
    {
        TutoresSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    }
    case WM_SIZE:
    {
        TutoresSelect_RecarregarDadosTabela(hWnd);
    }
    case WM_ACTIVATE:
    {
        if (LOWORD(wParam) == WA_INACTIVE) {
            TutoresSelect_g_wasInactive = true; // Marca como inativa quando perde foco para outra janela
        }
        else if (LOWORD(wParam) != WA_INACTIVE && TutoresSelect_g_wasInactive) {
            //RecarregarDadosTabela(hWnd);
            //MessageBoxW(hWnd, L"A janela ganhou foco novamente!", L"Aviso", MB_OK | MB_ICONINFORMATION);
            TutoresSelect_g_wasInactive = false; // Resetar após exibir o popup
        }
        return 0;
    }
    break;

    case WM_KILLFOCUS:
    {
        TutoresSelect_g_wasInactive = true; // Marca como inativa quando perde foco para outro controle
        return 0;
    }
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
            TutoresSelect_g_scrollY = si.nPos;

            TutoresSelect_RecarregarDadosTabela(hWnd);
        }
        break;
    }
    case WM_MOUSEWHEEL:
    {
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

            TutoresSelect_RecarregarDadosTabela(hWnd);
        }
        return 0;
    }
    case WM_ERASEBKGND:
    {
        // Retornar TRUE para evitar que o sistema apague o fundo
        // Isso reduz o flicker durante o redesenho
        return 1;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pDraw = (LPDRAWITEMSTRUCT)lParam;
        HWND hBotao = pDraw->hwndItem;

        // Obter o idBotao do GWLP_USERDATA
        LONG_PTR idBotao = GetWindowLongPtr(hBotao, GWLP_USERDATA);

        if (pDraw->CtlID == TutoresSelect_OFFSET) {
            RECT rcButton = pDraw->rcItem;

            // 1. PRIMEIRO desenhar a borda
            UINT uState = DFCS_BUTTONPUSH;
            if (pDraw->itemState & ODS_SELECTED) {
                uState |= DFCS_PUSHED;
            }
            DrawFrameControl(pDraw->hDC, &rcButton, DFC_BUTTON, uState);

            // 2. REDUZIR a área interna para não sobrepor a borda
            InflateRect(&rcButton, -2, -2);  // Reduz para dentro

            if (idBotao == TutoresSelect_idNumeroUltimo) {
                // Botão especial (vermelho)
                HBRUSH hBrush = CreateSolidBrush(RGB(150, 150, 150));
                FillRect(pDraw->hDC, &rcButton, hBrush);
                DeleteObject(hBrush);

                SetTextColor(pDraw->hDC, RGB(255, 255, 255));
            }
            else {
                // Botão normal
                COLORREF bgColor = GetSysColor(COLOR_BTNFACE);
                HBRUSH hBrush = CreateSolidBrush(bgColor);
                FillRect(pDraw->hDC, &rcButton, hBrush);
                DeleteObject(hBrush);

                SetTextColor(pDraw->hDC, GetSysColor(COLOR_BTNTEXT));
            }

            // 3. AGORA desenhar o texto
            SetBkMode(pDraw->hDC, TRANSPARENT);

            WCHAR buffer[32];
            swprintf(buffer, 32, L"%ld", (long)idBotao);

            DrawText(pDraw->hDC, buffer, -1, &rcButton,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        return TRUE;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
LRESULT CALLBACK WndProcSelectTutores(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc, 10, 10, L"Esta é a nova janela!", 21);
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
BOOL Init(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProcTutoresSelect;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SelectClass";
    return RegisterClassW(&wc);
}