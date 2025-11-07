#include "TutoresConsultar.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "TutoresFuncoes.h"
#include <string>
#include "TutoresSelect.h"
#include "sqlite3.h"
#include <vector>

// Variáveis externas para scroll
extern int TutoresSelect_g_scrollY;
extern int TutoresSelect_g_clientHeight;
extern int TutoresSelect_g_contentHeight;
extern int TutoresSelect_g_scrollX;
extern int TutoresSelect_g_clientWidth;
extern int TutoresSelect_g_contentWidth;

std::vector<std::vector<std::wstring>> TutoresSelect_g_tableDataConsulta;

// Função auxiliar para converter de UTF-8 (char*) para std::wstring (UTF-16)
std::wstring TutoresSelect_utf8_to_wstring_consulta(const char* str) {
    if (!str) return L"NULL";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstr(size_needed - 1, 0); // -1 para não incluir o caractere nulo
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size_needed);
    return wstr;
}

int TutoresSelect_sqlite_callback_consulta(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::vector<std::wstring>>* table = static_cast<std::vector<std::vector<std::wstring>>*>(data);
    // Primeira chamada: adicionar cabeçalhos (nomes das colunas)
    if (table->empty()) {
        std::vector<std::wstring> headers;
        for (int i = 0; i < argc; i++) {
            headers.push_back(azColName[i] ? TutoresSelect_utf8_to_wstring_consulta(azColName[i]) : L"NULL");
        }
        table->push_back(headers);
    }

    // Adicionar linha de dados
    std::vector<std::wstring> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? TutoresSelect_utf8_to_wstring_consulta(argv[i]) : L"NULL");
    }
    table->push_back(row);

    return 0;
}

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcTutoresRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para configurar scroll bars
void TutoresSelect_ConfigurarScrollBarsConsulta(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    TutoresSelect_g_clientHeight = rect.bottom - rect.top;
    TutoresSelect_g_clientWidth = rect.right - rect.left;

    // Calcular altura total do conteúdo baseado na tabela
    int cellHeight = 32;
    TutoresSelect_g_contentHeight = 24 * cellHeight + 100; // 22 colunas + margem
    TutoresSelect_g_contentWidth = 2000; // Largura fixa para conteúdo largo

    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    // Scroll vertical
    si.nMin = 0;
    si.nMax = TutoresSelect_g_contentHeight;
    si.nPage = TutoresSelect_g_clientHeight;
    si.nPos = TutoresSelect_g_scrollY;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    // Scroll horizontal
    si.nMin = 0;
    si.nMax = TutoresSelect_g_contentWidth;
    si.nPage = TutoresSelect_g_clientWidth;
    si.nPos = TutoresSelect_g_scrollX;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
}

// Função para registrar a classe da janela (pode ser chamada de outro lugar, como Pet.cpp)
BOOL RegisterTutoresReadClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcTutoresRead;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;  // Menu será definido dinamicamente
    wcex.lpszClassName = L"JanelaTutoresReadClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Read
LRESULT CALLBACK WndProcTutoresRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
    case WM_CREATE: {
        // Resetar scroll para garantir que comece do topo
        TutoresSelect_g_scrollY = 0;
        TutoresSelect_g_scrollX = 0;
        TutoresSelect_ConfigurarScrollBarsConsulta(hWnd);
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
            TutoresSelect_invalidateDrawing(hWnd);
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
            TutoresSelect_g_scrollY = 0;
            TutoresSelect_g_scrollX = 0;
        }

        oldWidth = newWidth;
        oldHeight = newHeight;

        TutoresSelect_g_clientWidth = newWidth;
        TutoresSelect_g_clientHeight = newHeight;
        SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
        TutoresSelect_ConfigurarScrollBarsConsulta(hWnd);
        SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
        TutoresSelect_invalidateDrawing(hWnd);
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
            TutoresSelect_invalidateDrawing(hWnd);
            UpdateWindow(hWnd);
        }
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        TutoresSelect_fonte(L"Font", RGB(0, 0, 0), hdc);

        // Texto de exemplo
        RECT rect;
        GetClientRect(hWnd, &rect);

        // Fundo branco
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
        TutoresSelect_g_tableDataConsulta.clear();

        // Abrir ou criar o banco de dados (código original mantido)
        char* errMsg = 0;
        sqlite3* db = nullptr;
        std::string dbPath = GetAppDataPath() + "pet.db";

        // Cria a pasta se não existir
        CreateDirectoryA(dbPath.substr(0, dbPath.find_last_of('\\')).c_str(), NULL);

        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc == SQLITE_OK) {
            // Supondo que TutoresSelect_idRecord é um long long/int, convertemos para string:
            std::string idRecordStr = std::to_string(TutoresSelect_idRecord);

            // 2. Constrói a consulta usando std::string para concatenação segura
            std::string sqlSelectConsulta_std =
                "SELECT * FROM Tutores WHERE ID = " + idRecordStr + "; "; // Assumindo que a coluna é 'ID'

            // 3. Se a sua função de banco de dados exigir 'const char*', converta:
            const char* sqlSelectConsulta = sqlSelectConsulta_std.c_str();

            rc = sqlite3_exec(db, sqlSelectConsulta, TutoresSelect_sqlite_callback_consulta, &TutoresSelect_g_tableDataConsulta, &errMsg);
            if (rc != SQLITE_OK) {
                if (errMsg) {
                    // Converte char* para wchar_t* corretamente
                    size_t len = strlen(errMsg) + 1;
                    std::wstring wErrMsg(len, L'\0');
                    mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                    // Remove o caractere nulo extra do final
                    wErrMsg.resize(wcslen(wErrMsg.c_str()));
                    TutoresSelect_g_tableDataConsulta.push_back({ L"Erro", wErrMsg });
                }
                else {
                    TutoresSelect_g_tableDataConsulta.push_back({ L"Erro", L"Desconhecido" });
                }
                if (errMsg) sqlite3_free(errMsg);
            }
            sqlite3_close(db);
        }
        else {
           // TutoresSelect_g_tableDataConsulta.push_back({ L"Erro", L"Não foi possível abrir o banco" });
        }

        if (TutoresSelect_g_tableDataConsulta.empty()) {
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }

        // Obter dimensões da janela
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;
        int height = rect.bottom - rect.top;

        // Configurar a tabela com scroll
        int cellHeight = 32;  // Altura de cada célula
        int numColumns = TutoresSelect_g_tableDataConsulta.empty() ? 0 : TutoresSelect_g_tableDataConsulta[0].size() + 3;
        int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
        int startY = 40 - TutoresSelect_g_scrollY;  // Posição Y com scroll
        int startX = 22 - TutoresSelect_g_scrollX;  // Posição X com scroll

        //Título
        TutoresSelect_windowsTitle(hdc, startX, startY - 20, L"CONSULTAR TUTOR", 15);

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

        for (size_t col = 0; col < 9; col++) {
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

            for (size_t row = 0; row < TutoresSelect_g_tableDataConsulta.size(); row++) {
                if (TutoresSelect_g_tableDataConsulta[row][0] == std::to_wstring(TutoresSelect_idRecord) || row == 0) {

                    int xPos;
                    int yPos;

                    if (row == 0) {
                        xPos = startX + 10;
                        yPos = startY + colNumber * cellHeight + 7;
                        TutoresSelect_fonte(L"Header", RGB(0, 0, 0), hdc);
                    }
                    else {
                        xPos = startX + cellWidth + 60;
                        yPos = startY + colNumber * cellHeight + 7;
                        TutoresSelect_fonte(L"Font", RGB(0, 0, 0), hdc);
                    }

                    if (TutoresSelect_g_tableDataConsulta[row][col] == L"ID") {
                        TextOut(hdc, xPos, yPos, L"ID:", 3);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Nome_do_Pet") {
                        TextOut(hdc, xPos, yPos, L"Nome do Pet:", 12);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Raca") {
                        TextOut(hdc, xPos, yPos, L"Raça:", 5);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Nome_do_Tutor") {
                        TextOut(hdc, xPos, yPos, L"Nome do Tutor:", 14);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"CEP") {
                        TextOut(hdc, xPos, yPos, L"CEP:", 4);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Cor") {
                        TextOut(hdc, xPos, yPos, L"Cor:", 4);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Idade") {
                        TextOut(hdc, xPos, yPos, L"Idade:", 6);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Peso") {
                        TextOut(hdc, xPos, yPos, L"Peso:", 5);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Sexo") {
                        TextOut(hdc, xPos, yPos, L"Sexo:", 5);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Castrado") {
                        TextOut(hdc, xPos, yPos, L"Castrado:", 9);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Endereco") {
                        TextOut(hdc, xPos, yPos, L"Endereço:", 9);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Ponto_de_referencia") {
                        TextOut(hdc, xPos, yPos, L"Ponto de Referência:", 20);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Banho") {
                        TextOut(hdc, xPos, yPos, L"Banho:", 6);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Tosa") {
                        TextOut(hdc, xPos, yPos, L"Tosa:", 5);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Obs_Tosa") {
                        TextOut(hdc, xPos, yPos, L"Observação:", 11);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Parasitas") {
                        TextOut(hdc, xPos, yPos, L"Parasitas:", 10);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Lesoes") {
                        TextOut(hdc, xPos, yPos, L"Lesões:", 7);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Obs_Lesoes") {
                        TextOut(hdc, xPos, yPos, L"Observação:", 11);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Telefone") {
                        TextOut(hdc, xPos, yPos, L"Telefone:", 9);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"CPF") {
                        TextOut(hdc, xPos, yPos, L"CPF:", 4);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Appointment_Date") {
                        TextOut(hdc, xPos, yPos, L"Data do Agendamento:", 20);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Appointment_Hour") {
                        TextOut(hdc, xPos, yPos, L"Hora do Agendamento:", 20);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Date") {
                        TextOut(hdc, xPos, yPos, L"Data do Registro:", 17);
                    }
                    else if (TutoresSelect_g_tableDataConsulta[row][col] == L"Hour") {
                        TextOut(hdc, xPos, yPos, L"Hora do Registro:", 17);
                    }
                    else {
                        TextOut(hdc, xPos, yPos, TutoresSelect_g_tableDataConsulta[row][col].c_str(),
                            static_cast<int>(TutoresSelect_g_tableDataConsulta[row][col].length()));
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
        TutoresSelect_Shortcuts(hWnd, message, wParam, lParam);
        break;
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
    case WM_DESTROY:
    {
        TutoresSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
LRESULT CALLBACK NewWndProcTutoresRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        TutoresSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
BOOL InitTutoresRead(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProcTutoresRead;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ReadClass";
    return RegisterClassW(&wc);
}