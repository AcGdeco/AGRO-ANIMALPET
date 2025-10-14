#include "Consultar.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "Pet.h"
#include <string>
#include "Select.h"
#include "sqlite3.h"
#include <vector>

// Variáveis externas para scroll
extern int g_scrollY;
extern int g_clientHeight;
extern int g_contentHeight;
extern int g_scrollX;
extern int g_clientWidth;
extern int g_contentWidth;

std::vector<std::vector<std::wstring>> g_tableDataConsulta;

// Função auxiliar para converter de UTF-8 (char*) para std::wstring (UTF-16)
std::wstring utf8_to_wstring_consulta(const char* str) {
    if (!str) return L"NULL";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstr(size_needed - 1, 0); // -1 para não incluir o caractere nulo
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size_needed);
    return wstr;
}

int sqlite_callback_consulta(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::vector<std::wstring>>* table = static_cast<std::vector<std::vector<std::wstring>>*>(data);
    // Primeira chamada: adicionar cabeçalhos (nomes das colunas)
    if (table->empty()) {
        std::vector<std::wstring> headers;
        for (int i = 0; i < argc; i++) {
            headers.push_back(azColName[i] ? utf8_to_wstring_consulta(azColName[i]) : L"NULL");
        }
        table->push_back(headers);
    }

    // Adicionar linha de dados
    std::vector<std::wstring> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? utf8_to_wstring_consulta(argv[i]) : L"NULL");
    }
    table->push_back(row);

    return 0;
}

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para configurar scroll bars
void ConfigurarScrollBarsConsulta(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    g_clientHeight = rect.bottom - rect.top;
    g_clientWidth = rect.right - rect.left;

    // Calcular altura total do conteúdo baseado na tabela
    int cellHeight = 32;
    g_contentHeight = 22 * cellHeight + 100; // 22 colunas + margem
    g_contentWidth = 2000; // Largura fixa para conteúdo largo

    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    // Scroll vertical
    si.nMin = 0;
    si.nMax = g_contentHeight;
    si.nPage = g_clientHeight;
    si.nPos = g_scrollY;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    // Scroll horizontal
    si.nMin = 0;
    si.nMax = g_contentWidth;
    si.nPage = g_clientWidth;
    si.nPos = g_scrollX;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
}

// Função para registrar a classe da janela (pode ser chamada de outro lugar, como Pet.cpp)
BOOL RegisterReadClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcRead;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;  // Menu será definido dinamicamente
    wcex.lpszClassName = L"JanelaReadClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Read
LRESULT CALLBACK WndProcRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
    case WM_CREATE: {
        // Resetar scroll para garantir que comece do topo
        g_scrollY = 0;
        g_scrollX = 0;
        ConfigurarScrollBarsConsulta(hWnd);
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
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        break;
    }

    case WM_SIZE: {
        int newWidth = LOWORD(lParam);
        int newHeight = HIWORD(lParam);

        // Verificar se é uma mudança significativa de tamanho (maximizar/restaurar)
        static int oldWidth = 0;
        static int oldHeight = 0;

        if ((newWidth > oldWidth * 1.5) || (newHeight > oldHeight * 1.5) ||
            (newWidth < oldWidth * 0.7) || (newHeight < oldHeight * 0.7)) {
            // Mudança significativa - resetar scroll para o topo
            g_scrollY = 0;
            g_scrollX = 0;
        }

        oldWidth = newWidth;
        oldHeight = newHeight;

        g_clientWidth = newWidth;
        g_clientHeight = newHeight;
        ConfigurarScrollBarsConsulta(hWnd);
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
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        fonte(L"Font", RGB(0, 0, 0), hdc);

        // Texto de exemplo
        RECT rect;
        GetClientRect(hWnd, &rect);

        // Fundo branco
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // Consultar o banco apenas se a tabela estiver vazia
        sqlite3* db;
        char* errMsg = 0;
        int rc = sqlite3_open("pet.db", &db);
        if (rc == SQLITE_OK) {
            const char* sqlSelectConsulta = "SELECT * FROM Pets;";
            rc = sqlite3_exec(db, sqlSelectConsulta, sqlite_callback_consulta, &g_tableDataConsulta, &errMsg);
            if (rc != SQLITE_OK) {
                if (errMsg) {
                    // Converte char* para wchar_t* corretamente
                    size_t len = strlen(errMsg) + 1;
                    std::wstring wErrMsg(len, L'\0');
                    mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                    // Remove o caractere nulo extra do final
                    wErrMsg.resize(wcslen(wErrMsg.c_str()));
                    g_tableDataConsulta.push_back({ L"Erro", wErrMsg });
                }
                else {
                    g_tableDataConsulta.push_back({ L"Erro", L"Desconhecido" });
                }
                if (errMsg) sqlite3_free(errMsg);
            }
            sqlite3_close(db);
        }
        else {
            g_tableDataConsulta.push_back({ L"Erro", L"Não foi possível abrir o banco" });
        }        

        // Obter dimensões da janela
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;
        int height = rect.bottom - rect.top;

        // Configurar a tabela com scroll
        int cellHeight = 32;  // Altura de cada célula
        int numColumns = g_tableDataConsulta.empty() ? 0 : g_tableDataConsulta[0].size() + 3;
        int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
        int startY = 40 - g_scrollY;  // Posição Y com scroll
        int startX = 22 - g_scrollX;  // Posição X com scroll

        //Título
        windowsTitle(hdc, startX, startY - 20, L"CONSULTAR AGENDAMENTO", 21);

        // Desenhar a grade
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        // Desenhar fundos alternados para as linhas
        HBRUSH hBrushHeader = CreateSolidBrush(RGB(150, 150, 150));
        HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrushWhite);

        // Desenhar o texto nas células
        SetBkMode(hdc, TRANSPARENT);
        int colNumber = 0;
        int rowNumber = 0;

        for (size_t col = 0; col < 24; col++) {
            colNumber++;

            HBRUSH hCurrentBrush = (col % 2 == 0) ? hBrushGray : hBrushWhite;
            SelectObject(hdc, hCurrentBrush);

            // Desenhar o fundo da linha (com scroll)
            RECT rowRect = {
                startX,
                startY + static_cast<int>(colNumber) * cellHeight,
                startX + width,
                startY + (static_cast<int>(colNumber) + 1) * cellHeight
            };
            FillRect(hdc, &rowRect, hCurrentBrush);

            for (size_t row = 0; row < g_tableDataConsulta.size(); row++) {
                if (g_tableDataConsulta[row][0] == std::to_wstring(idRecord) || row == 0) {

                    int xPos;
                    int yPos;

                    if (row == 0) {
                        xPos = startX + 10;
                        yPos = startY + colNumber * cellHeight + 7;
                        fonte(L"Header", RGB(0, 0, 0), hdc);
                    }
                    else {
                        xPos = startX + cellWidth + 60;
                        yPos = startY + colNumber * cellHeight + 7;
                        fonte(L"Font", RGB(0, 0, 0), hdc);
                    }

                    if (g_tableDataConsulta[row][col] == L"ID") {
                        TextOut(hdc, xPos, yPos, L"ID:", 3);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Nome_do_Pet") {
                        TextOut(hdc, xPos, yPos, L"Nome do Pet:", 12);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Raca") {
                        TextOut(hdc, xPos, yPos, L"Raça:", 5);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Nome_do_Tutor") {
                        TextOut(hdc, xPos, yPos, L"Nome do Tutor:", 14);
                    }
                    else if (g_tableDataConsulta[row][col] == L"CEP") {
                        TextOut(hdc, xPos, yPos, L"CEP:", 4);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Cor") {
                        TextOut(hdc, xPos, yPos, L"Cor:", 4);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Idade") {
                        TextOut(hdc, xPos, yPos, L"Idade:", 6);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Peso") {
                        TextOut(hdc, xPos, yPos, L"Peso:", 5);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Sexo") {
                        TextOut(hdc, xPos, yPos, L"Sexo:", 5);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Castrado") {
                        TextOut(hdc, xPos, yPos, L"Castrado:", 9);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Endereco") {
                        TextOut(hdc, xPos, yPos, L"Endereço:", 9);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Ponto_de_referencia") {
                        TextOut(hdc, xPos, yPos, L"Ponto de Referência:", 20);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Banho") {
                        TextOut(hdc, xPos, yPos, L"Banho:", 6);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Tosa") {
                        TextOut(hdc, xPos, yPos, L"Tosa:", 5);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Obs_Tosa") {
                        TextOut(hdc, xPos, yPos, L"Observação:", 11);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Parasitas") {
                        TextOut(hdc, xPos, yPos, L"Parasitas:", 10);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Lesoes") {
                        TextOut(hdc, xPos, yPos, L"Lesões:", 7);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Obs_Lesoes") {
                        TextOut(hdc, xPos, yPos, L"Observação:", 11);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Telefone") {
                        TextOut(hdc, xPos, yPos, L"Telefone:", 9);
                    }
                    else if (g_tableDataConsulta[row][col] == L"CPF") {
                        TextOut(hdc, xPos, yPos, L"CPF:", 4);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Appointment_Date") {
                        TextOut(hdc, xPos, yPos, L"Data do Agendamento:", 20);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Appointment_Hour") {
                        TextOut(hdc, xPos, yPos, L"Hora do Agendamento:", 20);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Date") {
                        TextOut(hdc, xPos, yPos, L"Data do Registro:", 17);
                    }
                    else if (g_tableDataConsulta[row][col] == L"Hour") {
                        TextOut(hdc, xPos, yPos, L"Hora do Registro:", 17);
                    }
                    else {
                        TextOut(hdc, xPos, yPos, g_tableDataConsulta[row][col].c_str(),
                            static_cast<int>(g_tableDataConsulta[row][col].length()));
                    }
                }
                rowNumber++;
            }
        }

        // Limpar recursos
        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrushHeader);
        DeleteObject(hBrushWhite);
        DeleteObject(hBrushGray);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_KEYDOWN:
    {
        Shortcuts(hWnd, message, wParam, lParam);
        break;
    }

    case WM_DESTROY:
    {
        windowClose(hWnd, message, wParam, lParam);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
LRESULT CALLBACK NewWndProcRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc, 10, 10, L"Esta é a nova janela Read!", 21);
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
BOOL InitRead(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProcRead;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ReadClass";
    return RegisterClassW(&wc);
}